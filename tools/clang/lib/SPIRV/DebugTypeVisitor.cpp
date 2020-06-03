//===--- LowerTypeVisitor.cpp - AST type to SPIR-V type impl -----*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <sstream>

#include "DebugTypeVisitor.h"
#include "clang/SPIRV/SpirvBuilder.h"
#include "clang/SPIRV/SpirvModule.h"

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

SpirvDebugTypeComposite *
DebugTypeVisitor::lowerCbufferDebugType(const StructType *type,
                                        const SourceLocation &loc) {
  const auto &sm = astContext.getSourceManager();
  uint32_t line = sm.getPresumedLineNumber(loc);
  uint32_t column = sm.getPresumedColumnNumber(loc);
  StringRef linkageName = type->getName();

  // TODO: Update linkageName using astContext.createMangleContext().
  std::string name = type->getName();

  // TODO: Update parent, size, flags, and tag information correctly.
  RichDebugInfo *debugInfo = &spvContext.getDebugInfo().begin()->second;
  const char *file = sm.getPresumedLoc(loc).getFilename();
  if (file)
    debugInfo = &spvContext.getDebugInfo()[file];
  auto *dbgTyComposite =
      dyn_cast<SpirvDebugTypeComposite>(spvContext.getDebugTypeComposite(
          type, name, debugInfo->source, line, column,
          /* parent */ debugInfo->compilationUnit, linkageName,
          /* size */ 0, 3u, 1u));

  auto &members = dbgTyComposite->getMembers();
  for (auto &field : type->getFields()) {
    uint32_t offset = UINT32_MAX;
    if (field.offset.hasValue())
      offset = *field.offset;

    // TODO: Replace 2u and 3u with valid flags when debug info extension is
    // placed in SPIRV-Header.
    auto *debugInstr =
        dyn_cast<SpirvDebugInstruction>(spvContext.getDebugTypeMember(
            field.name, field.type, debugInfo->source, line, column,
            dbgTyComposite,
            /* flags */ 3u, offset, /* value */ nullptr));
    assert(debugInstr);
    setDefaultDebugInfo(debugInstr);
    members.push_back(debugInstr);
  }

  setDefaultDebugInfo(dbgTyComposite);
  return dbgTyComposite;
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

bool DebugTypeVisitor::lowerDebugTypeMember(SpirvDebugTypeMember *debugMember,
                                            uint32_t *sizeInBits,
                                            uint32_t *offsetInBits) {
  assert(sizeInBits);
  assert(offsetInBits);

  auto *ty = lowerToDebugType(debugMember->getSpirvType());
  if (ty == nullptr)
    return false;

  SpirvDebugType *memberTy = dyn_cast<SpirvDebugType>(ty);
  debugMember->setType(memberTy);

  uint32_t memberSizeInBits = memberTy->getSizeInBits();
  uint32_t memberOffset = debugMember->getOffset();
  if (memberOffset == UINT32_MAX)
    memberOffset = *offsetInBits;
  debugMember->updateOffsetAndSize(memberOffset, memberSizeInBits);

  *offsetInBits = memberOffset + memberSizeInBits;
  if (*sizeInBits < *offsetInBits)
    *sizeInBits = *offsetInBits;

  return true;
}

SpirvDebugInstruction *
DebugTypeVisitor::lowerToDebugTypeComposite(const SpirvType *type) {
  // DebugTypeComposite is already lowered by LowerTypeVisitor,
  // but it is not completely lowered.
  // We have to update member information including offset and size.
  auto *instr =
      dyn_cast<SpirvDebugTypeComposite>(spvContext.getDebugType(type));
  if (!lowerDebugTypeTemplate(instr)) {
    emitError("Lowering DebugTypeComposite for SpirvType %0 fails")
        << type->getName();
    return nullptr;
  }
  if (instr->getFullyLowered())
    return instr;

  uint32_t sizeInBits = 0;
  uint32_t offsetInBits = 0;
  auto &members = instr->getMembers();
  for (auto *member : members) {
    auto *debugMember = dyn_cast<SpirvDebugTypeMember>(member);
    if (!debugMember) {
      if (!lowerDebugTypeFunctionForMemberFunction(member))
        return nullptr;
      continue;
    }

    if (!lowerDebugTypeMember(debugMember, &sizeInBits, &offsetInBits))
      return nullptr;
  }
  instr->setSizeInBits(sizeInBits);
  instr->setFullyLowered();
  return instr;
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
      counts = dbgArrType->getElementCount();
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
        spirvType, elemDebugType,
        llvm::SmallVector<uint32_t, 2>(
            {matType->numRows(), matType->numCols()}));
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
      if (!spirvType) {
        // If a debug instruction does not have QualType, LowerTypeVisitor
        // does not invoke lowerDebugTypeComposite() even though the type is a
        // composite type. For example, cbuffer does not have QualType, but it
        // has StructType. We have to prepare a DebugTypeComposite for such
        // StructType before calling
        // DebugTypeVisitor::lowerToDebugTypeComposite().
        if (auto *debugGlobalVar = dyn_cast<SpirvDebugGlobalVariable>(instr)) {
          auto *varType = debugGlobalVar->getVariable()->getResultType();
          assert(varType &&
                 "Global variables must be lowered by LowerTypeVisitor");
          auto *ptrType = dyn_cast<SpirvPointerType>(varType);
          assert(ptrType && "OpVariable must have a pointer type");
          spirvType = ptrType->getPointeeType();
          debugGlobalVar->setDebugSpirvType(spirvType);
          if (auto *structType = dyn_cast<StructType>(spirvType))
            lowerCbufferDebugType(structType, instr->getSourceLocation());
        }
      }
      if (spirvType) {
        SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
        debugInstr->setDebugType(debugType);
      }
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