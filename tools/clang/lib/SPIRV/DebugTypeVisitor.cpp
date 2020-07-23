//===--- DebugTypeVisitor.cpp - SPIR-V type to debug type impl ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <sstream>

#include "DebugTypeVisitor.h"
#include "LowerTypeVisitor.h"
#include "clang/SPIRV/SpirvBuilder.h"
#include "clang/SPIRV/SpirvModule.h"

static const uint32_t kUninitializedOffset = UINT32_MAX;

namespace clang {
namespace spirv {

void addTemplateTypeAndItsParamsToModule(SpirvModule *module,
                                         SpirvDebugTypeTemplate *tempType) {
  assert(module && "module is nullptr");
  assert(tempType && "tempType is nullptr");

  for (auto *param : tempType->getParams()) {
    module->addDebugInfo(param);
  }
  module->addDebugInfo(tempType);
}

void DebugTypeVisitor::setDefaultDebugInfo(SpirvDebugInstruction *instr) {
  instr->setAstResultType(astContext.VoidTy);
  instr->setResultType(context.getVoidType());
  instr->setInstructionSet(spvBuilder.getOpenCLDebugInfoExtInstSet());
}

SpirvDebugInfoNone *DebugTypeVisitor::getDebugInfoNone() {
  auto *debugNone = spvBuilder.getOrCreateDebugInfoNone();
  setDefaultDebugInfo(debugNone);
  return debugNone;
}

SpirvDebugTypeComposite *DebugTypeVisitor::createDebugTypeComposite(
    const StructType *type, const SourceLocation &loc, uint32_t tag) {
  const auto &sm = astContext.getSourceManager();
  uint32_t line = sm.getPresumedLineNumber(loc);
  uint32_t column = sm.getPresumedColumnNumber(loc);
  StringRef linkageName = type->getName();

  // TODO: Update linkageName using astContext.createMangleContext().
  std::string name = type->getName();

  // TODO: Update size information correctly.
  RichDebugInfo *debugInfo = &spvContext.getDebugInfo().begin()->second;
  const char *file = sm.getPresumedLoc(loc).getFilename();
  if (file)
    debugInfo = &spvContext.getDebugInfo()[file];
  return spvContext.getDebugTypeComposite(
      type, name, debugInfo->source, line, column,
      /* parent */ debugInfo->compilationUnit, linkageName, 3u, tag);
}

void DebugTypeVisitor::addDebugTypeMember(
    SpirvDebugTypeComposite *debugTypeComposite, const StructType *type,
    const SourceLocation &loc) {
  const auto &sm = astContext.getSourceManager();
  uint32_t line = sm.getPresumedLineNumber(loc);
  uint32_t column = sm.getPresumedColumnNumber(loc);

  // Add DebugTypeMemeber.
  auto &members = debugTypeComposite->getMembers();
  for (auto &field : type->getFields()) {
    uint32_t offsetInBits = kUninitializedOffset;
    if (field.offset.hasValue())
      offsetInBits = *field.offset * 8;

    // TODO: Replace 2u and 3u with valid flags when debug info extension is
    // placed in SPIRV-Header.
    auto *debugInstr =
        dyn_cast<SpirvDebugInstruction>(spvContext.getDebugTypeMember(
            field.name, field.type, debugTypeComposite->getSource(), line,
            column, debugTypeComposite,
            /* flags */ 3u, offsetInBits, /* value */ nullptr));
    assert(debugInstr);
    setDefaultDebugInfo(debugInstr);
    members.push_back(debugInstr);
  }

  // Lower the debug type of DebugTypeMemeber and set size and offset.
  uint32_t sizeInBits = 0;
  uint32_t offsetInBits = 0;
  for (auto *member : members) {
    auto *debugMember = dyn_cast<SpirvDebugTypeMember>(member);
    assert(debugMember != nullptr);
    debugMember->setDebugType(lowerToDebugType(debugMember->getSpirvType()));

    uint32_t memberSizeInBits = debugMember->getDebugType()->getSizeInBits();
    uint32_t memberOffsetInBits = debugMember->getOffsetInBits();
    if (memberOffsetInBits == kUninitializedOffset)
      memberOffsetInBits = offsetInBits;
    debugMember->updateOffsetAndSize(memberOffsetInBits, memberSizeInBits);

    offsetInBits = memberOffsetInBits + memberSizeInBits;
    if (sizeInBits < offsetInBits)
      sizeInBits = offsetInBits;
  }
  debugTypeComposite->setSizeInBits(sizeInBits);
}

bool DebugTypeVisitor::lowerDebugTypeTemplate(SpirvDebugTypeComposite *instr) {
  if (instr == nullptr)
    return false;

  auto *tempType = instr->getTypeTemplate();
  // It is not a composite type for a resource. It is not an error.
  if (tempType == nullptr)
    return true;

  auto &tempParams = tempType->getParams();
  for (auto &t : tempParams) {
    auto *loweredParam = lowerToDebugType(t->getSpirvType());
    if (loweredParam == nullptr)
      return false;
    t->setActualType(dyn_cast<SpirvDebugType>(loweredParam));
    if (!t->getValue()) {
      auto *debugNone = getDebugInfoNone();
      if (debugNone == nullptr)
        return false;
      t->setValue(debugNone);
    }
    setDefaultDebugInfo(t);
  }
  setDefaultDebugInfo(tempType);

  return true;
}

bool DebugTypeVisitor::lowerDebugTypeFunctionForMemberFunction(
    SpirvDebugInstruction *instr) {
  auto *fn = dyn_cast<SpirvDebugFunction>(instr);
  if (fn == nullptr) {
    emitError("Debug instruction %0 is not a DebugFunction")
        << instr->getDebugName();
    return false;
  }

  if (const auto *fnType = fn->getFunctionType()) {
    fn->setDebugType(lowerToDebugType(fnType));
    setDefaultDebugInfo(fn);
  }
  if (!fn->getSpirvFunction()) {
    auto *debugNone = getDebugInfoNone();
    fn->setDebugInfoNone(debugNone);
  }
  return true;
}

SpirvDebugInstruction *
DebugTypeVisitor::lowerToDebugTypeComposite(const SpirvType *type) {
  if (const auto *recordType = spvContext.getRecordType(type)) {
    const auto *decl = recordType->getDecl();
    uint32_t tag = 1;
    if (decl->isStruct())
      tag = 1;
    else if (decl->isClass())
      tag = 0;
    else if (decl->isUnion())
      tag = 2;
    else
      assert(!"DebugTypeComposite must be a struct, class, or union.");
    auto *debugTypeComposite =
        createDebugTypeComposite(type, decl->getLocStart(), tag);
    if (const auto *templateType = type->getAs<TemplateSpecializationType>()) {
      return lowerDebugTypeTemplate(templateType, debugTypeComposite);
    } else {
      addDebugTypeMember(debugTypeComposite, type, decl->getLocStart());
      return debugTypeComposite;
    }
  } else if (const auto *decl = spvContext.getDeclForSpirvType(type)) {
    auto *debugTypeComposite =
        createDebugTypeComposite(type, decl->getLocStart(), 1u);
    addDebugTypeMember(debugTypeComposite, type, decl->getLocStart());
    return debugTypeComposite;
  }
  assert(false && "Unreachable");
}

SpirvDebugInstruction *
DebugTypeVisitor::lowerToDebugType(const SpirvType *spirvType) {
  SpirvDebugInstruction *debugType = nullptr;

  switch (spirvType->getKind()) {
  case SpirvType::TK_Bool: {
    llvm::StringRef name = "bool";
    // TODO: Should we use 1 bit for booleans or 32 bits?
    uint32_t size = 32;
    // TODO: Use enums rather than uint32_t.
    uint32_t encoding = 2u;
    SpirvConstant *sizeInstruction = spvBuilder.getConstantInt(
        astContext.UnsignedIntTy, llvm::APInt(32, size));
    sizeInstruction->setResultType(spvContext.getUIntType(32));
    debugType = spvContext.getDebugTypeBasic(spirvType, name, sizeInstruction,
                                             encoding);
    break;
  }
  case SpirvType::TK_Integer: {
    auto *intType = dyn_cast<IntegerType>(spirvType);
    const uint32_t size = intType->getBitwidth();
    const bool isSigned = intType->isSignedInt();
    SpirvConstant *sizeInstruction = spvBuilder.getConstantInt(
        astContext.UnsignedIntTy, llvm::APInt(32, size));
    sizeInstruction->setResultType(spvContext.getUIntType(32));
    // TODO: Use enums rather than uint32_t.
    uint32_t encoding = isSigned ? 4u : 6u;
    std::string debugName = "";
    if (size == 32) {
      debugName = isSigned ? "int" : "uint";
    } else {
      std::ostringstream stream;
      stream << (isSigned ? "int" : "uint") << size << "_t";
      debugName = stream.str();
    }
    debugType = spvContext.getDebugTypeBasic(spirvType, debugName,
                                             sizeInstruction, encoding);
    break;
  }
  case SpirvType::TK_Float: {
    auto *floatType = dyn_cast<FloatType>(spirvType);
    const uint32_t size = floatType->getBitwidth();
    SpirvConstant *sizeInstruction = spvBuilder.getConstantInt(
        astContext.UnsignedIntTy, llvm::APInt(32, size));
    sizeInstruction->setResultType(spvContext.getUIntType(32));
    // TODO: Use enums rather than uint32_t.
    uint32_t encoding = 3u;
    std::string debugName = "";
    if (size == 32) {
      debugName = "float";
    } else {
      std::ostringstream stream;
      stream << "float" << size << "_t";
      debugName = stream.str();
    }
    debugType = spvContext.getDebugTypeBasic(spirvType, debugName,
                                             sizeInstruction, encoding);
    break;
  }
  case SpirvType::TK_Image:
  case SpirvType::TK_Sampler:
  case SpirvType::TK_Struct: {
    debugType = lowerToDebugTypeComposite(spirvType);
    break;
  }
  // TODO: Add DebugTypeComposite for class and union.
  // TODO: Add DebugTypeEnum.
  case SpirvType::TK_Array: {
    auto *arrType = dyn_cast<ArrayType>(spirvType);
    SpirvDebugInstruction *elemDebugType =
        lowerToDebugType(arrType->getElementType());

    llvm::SmallVector<uint32_t, 4> counts;
    if (auto *dbgArrType = dyn_cast<SpirvDebugTypeArray>(elemDebugType)) {
      counts.insert(counts.end(), dbgArrType->getElementCount().begin(),
                    dbgArrType->getElementCount().end());
      elemDebugType = dbgArrType->getElementType();
    }
    counts.push_back(arrType->getElementCount());

    debugType = spvContext.getDebugTypeArray(spirvType, elemDebugType, counts);
    break;
  }
  case SpirvType::TK_Vector: {
    auto *vecType = dyn_cast<VectorType>(spirvType);
    SpirvDebugInstruction *elemDebugType =
        lowerToDebugType(vecType->getElementType());
    debugType = spvContext.getDebugTypeVector(spirvType, elemDebugType,
                                              vecType->getElementCount());
    break;
  }
  case SpirvType::TK_Matrix: {
    // TODO: I temporarily use a DebugTypeArray for a matrix type.
    // However, when the debug info extension supports matrix type
    // e.g., DebugTypeMatrix, we must replace DebugTypeArray with
    // DebugTypeMatrix.
    auto *matType = dyn_cast<MatrixType>(spirvType);
    SpirvDebugInstruction *elemDebugType =
        lowerToDebugType(matType->getElementType());
    debugType = spvContext.getDebugTypeArray(
        spirvType, elemDebugType, {matType->numRows(), matType->numCols()});
    break;
  }
  case SpirvType::TK_Pointer: {
    debugType = lowerToDebugType(
        dyn_cast<SpirvPointerType>(spirvType)->getPointeeType());
    break;
  }
  case SpirvType::TK_Function: {
    auto *fnType = dyn_cast<FunctionType>(spirvType);
    // Special case: There is no DebugType for void. So if the function return
    // type is void, we set it to nullptr.
    SpirvDebugType *returnType = nullptr;
    if (!isa<VoidType>(fnType->getReturnType())) {
      auto *loweredRetTy = lowerToDebugType(fnType->getReturnType());
      returnType = dyn_cast<SpirvDebugType>(loweredRetTy);
      assert(returnType && "Function return type info must be SpirvDebugType");
    }
    llvm::SmallVector<SpirvDebugType *, 4> params;
    for (const auto *paramType : fnType->getParamTypes()) {
      params.push_back(dyn_cast<SpirvDebugType>(lowerToDebugType(paramType)));
    }
    // TODO: Add mechanism to properly calculate the flags.
    // The info needed probably resides in clang::FunctionDecl.
    // This info can either be stored in the SpirvFunction class. Or,
    // alternatively the info can be stored in the SpirvContext.
    const uint32_t flags = 3u;
    debugType =
        spvContext.getDebugTypeFunction(spirvType, flags, returnType, params);
    break;
  }
  }

  if (!debugType) {
    emitError("Fail to lower SpirvType %0 to a debug type")
        << spirvType->getName();
    return nullptr;
  }

  setDefaultDebugInfo(debugType);
  return debugType;
}

bool DebugTypeVisitor::visitInstruction(SpirvInstruction *instr) {
  if (auto *debugInstr = dyn_cast<SpirvDebugInstruction>(instr)) {
    setDefaultDebugInfo(debugInstr);

    // The following instructions are the only debug instructions that contain a
    // debug type:
    // DebugGlobalVariable
    // DebugLocalVariable
    // DebugFunction
    if (isa<SpirvDebugGlobalVariable>(debugInstr) ||
        isa<SpirvDebugLocalVariable>(debugInstr)) {
      const SpirvType *spirvType = debugInstr->getDebugSpirvType();
      assert(spirvType != nullptr);
      SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
      debugInstr->setDebugType(debugType);
    }
    if (auto *debugFunction = dyn_cast<SpirvDebugFunction>(debugInstr)) {
      const SpirvType *spirvType =
          debugFunction->getSpirvFunction()->getFunctionType();
      if (spirvType) {
        SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
        debugInstr->setDebugType(debugType);
      }
    }
  }

  return true;
}

bool DebugTypeVisitor::visit(SpirvModule *module, Phase phase) {
  if (phase == Phase::Done) {
    // When the processing for all debug types is done, we need to take all the
    // debug types in the context and add their SPIR-V instructions to the
    // SPIR-V module.
    // Note that we don't add debug types to the module when we create them, as
    // there could be duplicates.
    for (const auto &typePair : spvContext.getDebugTypes()) {
      if (auto *tempType = dyn_cast<SpirvDebugTypeTemplate>(typePair.second)) {
        addTemplateTypeAndItsParamsToModule(module, tempType);
        continue;
      }

      module->addDebugInfo(typePair.second);

      // If SpirvDebugFunction is a member of this composite type and
      // it has FunctionType, it means DebugTypeVisitor lowers the
      // FunctionType to generate the debug function type info which is
      // not yet added to debug info of the module. We must add it now.
      if (auto *composite =
              dyn_cast<SpirvDebugTypeComposite>(typePair.second)) {
        if (auto *tempType = composite->getTypeTemplate()) {
          addTemplateTypeAndItsParamsToModule(module, tempType);
          continue;
        }

        auto &members = composite->getMembers();
        for (auto *member : members) {
          auto *fn = dyn_cast<SpirvDebugFunction>(member);
          if (!fn)
            continue;
          if (fn->getFunctionType())
            module->addDebugInfo(fn);
        }
      }
    }
    for (auto *type : spvContext.getTailDebugTypes()) {
      module->addDebugInfo(type);
    }
  }

  return true;
}

} // namespace spirv
} // namespace clang
