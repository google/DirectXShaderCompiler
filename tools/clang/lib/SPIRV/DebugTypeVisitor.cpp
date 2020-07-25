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
#include "clang/AST/DeclTemplate.h"
#include "clang/SPIRV/SpirvBuilder.h"
#include "clang/SPIRV/SpirvModule.h"

static const uint32_t kUnknownBitSize = 0;

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

void DebugTypeVisitor::addDebugTypeMembers(
    SpirvDebugTypeComposite *debugTypeComposite, const StructType *type) {
  llvm::SmallVector<SpirvDebugInstruction *, 4> members;
  uint32_t compositeSizeInBits = kUnknownBitSize;
  bool unknownPhysicalLayout = false;
  for (auto &field : type->getFields()) {
    uint32_t offsetInBits = kUnknownBitSize;
    if (!unknownPhysicalLayout && field.offset.hasValue())
      offsetInBits = *field.offset * 8;
    else
      unknownPhysicalLayout = true;

    // TODO: We are currently in the discussion about how to handle
    // a variable type with unknown physical layout. Add proper flags
    // or operations for variables with the unknown physical layout.
    // For example, we do not have physical layout for a local variable.

    // TODO: Replace 2u and 3u with valid flags when debug info extension is
    // placed in SPIRV-Header.
    auto *debugInstr = spvContext.getDebugTypeMember(
        field.name, lowerToDebugType(field.type),
        debugTypeComposite->getSource(), debugTypeComposite,
        /* flags */ 3u, offsetInBits, /* value */ nullptr);
    assert(debugInstr);
    setDefaultDebugInfo(debugInstr);
    members.push_back(debugInstr);

    if (offsetInBits == kUnknownBitSize || !field.sizeInBytes.hasValue()) {
      compositeSizeInBits = kUnknownBitSize;
    } else {
      compositeSizeInBits = offsetInBits + *field.sizeInBytes * 8;
    }
  }
  debugTypeComposite->setMembers(members);
  debugTypeComposite->setSizeInBits(compositeSizeInBits);
}

void DebugTypeVisitor::lowerDebugTypeMembers(
    SpirvDebugTypeComposite *debugTypeComposite, const StructType *type,
    const DeclContext *decl) {
  const auto &sm = astContext.getSourceManager();
  addDebugTypeMembers(debugTypeComposite, type);

  auto subDeclIter = decl->decls_begin();
  for (auto *member : debugTypeComposite->getMembers()) {
    // Skip member functions and "this" object.
    while (isa<CXXMethodDecl>(&*subDeclIter) ||
           isa<CXXRecordDecl>(&*subDeclIter)) {
      ++subDeclIter;
    }

    auto *debugTypeMember = dyn_cast<SpirvDebugTypeMember>(member);
    if (debugTypeMember == nullptr) {
      assert(false && "Only SpirvDebugTypeMember is expected for member "
                      "at this point");
      continue;
    }

    // Set line and column.
    const SourceLocation loc = subDeclIter->getLocStart();
    uint32_t line = sm.getPresumedLineNumber(loc);
    uint32_t column = sm.getPresumedColumnNumber(loc);
    debugTypeMember->SetLineAndColumn(line, column);
    ++subDeclIter;
  }

  // TODO: Add member functions.
}

SpirvDebugTypeTemplate *DebugTypeVisitor::lowerDebugTypeTemplate(
    const ClassTemplateSpecializationDecl *templateDecl,
    SpirvDebugTypeComposite *debugTypeComposite) {
  // Reuse already lowered DebugTypeTemplate.
  auto *debugTypeTemplate = spvContext.getDebugTypeTemplate(templateDecl);
  if (debugTypeTemplate != nullptr)
    return debugTypeTemplate;

  llvm::SmallVector<SpirvDebugTypeTemplateParameter *, 2> tempTypeParams;
  const auto &argList = templateDecl->getTemplateArgs();
  for (unsigned i = 0; i < argList.size(); ++i) {
    // Reuse already lowered DebugTypeTemplateParameter.
    auto *debugTypeTemplateParam =
        spvContext.getDebugTypeTemplateParameter(&argList[i]);
    if (debugTypeTemplateParam != nullptr) {
      tempTypeParams.push_back(debugTypeTemplateParam);
      continue;
    }

    // Lower DebugTypeTemplateParameter.
    const auto *spvType = spvTypeVisitor.lowerType(
        argList[i].getAsType(), currentDebugInstructionLayoutRule, llvm::None,
        debugTypeComposite->getSourceLocation());
    debugTypeTemplateParam = spvContext.createDebugTypeTemplateParameter(
        &argList[i], "TemplateParam", lowerToDebugType(spvType), nullptr,
        debugTypeComposite->getSource(), debugTypeComposite->getLine(),
        debugTypeComposite->getColumn());
    tempTypeParams.push_back(debugTypeTemplateParam);
    setDefaultDebugInfo(debugTypeTemplateParam);
  }

  debugTypeTemplate = spvContext.createDebugTypeTemplate(
      templateDecl, debugTypeComposite, tempTypeParams);
  setDefaultDebugInfo(debugTypeTemplate);
  return debugTypeTemplate;
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

SpirvDebugType *
DebugTypeVisitor::lowerToDebugTypeComposite(const SpirvType *type) {
  const StructType *structType = dyn_cast<StructType>(type);
  assert(structType != nullptr && "SpirvType must be a struct type");
  const auto *decl = spvContext.getDeclForSpirvType(structType);
  assert(decl != nullptr && "Lowering DebugTypeComposite needs DeclContext");

  uint32_t tag = 1u;
  if (const auto *recordDecl = dyn_cast<RecordDecl>(decl)) {
    if (recordDecl->isStruct())
      tag = 1;
    else if (recordDecl->isClass())
      tag = 0;
    else if (recordDecl->isUnion())
      tag = 2;
    else
      assert(!"DebugTypeComposite must be a struct, class, or union.");
  }
  SourceLocation loc = {};
  if (const auto *declDecl = dyn_cast<Decl>(decl))
    loc = declDecl->getLocation();
  auto *debugTypeComposite = createDebugTypeComposite(structType, loc, tag);

  if (const auto *templateDecl =
          dyn_cast<ClassTemplateSpecializationDecl>(decl)) {
    // The size of an opaque type must be DebugInfoNone and its name must
    // start with "@".
    debugTypeComposite->markOpaqueType(getDebugInfoNone());
    return lowerDebugTypeTemplate(templateDecl, debugTypeComposite);
  } else {
    lowerDebugTypeMembers(debugTypeComposite, structType, decl);
    return debugTypeComposite;
  }
}

SpirvDebugType *DebugTypeVisitor::lowerToDebugType(const SpirvType *spirvType) {
  SpirvDebugType *debugType = nullptr;

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
      currentDebugInstructionLayoutRule = debugInstr->getLayoutRule();
      const SpirvType *spirvType = debugInstr->getDebugSpirvType();
      assert(spirvType != nullptr);
      SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
      debugInstr->setDebugType(debugType);
    }
    if (auto *debugFunction = dyn_cast<SpirvDebugFunction>(debugInstr)) {
      currentDebugInstructionLayoutRule = SpirvLayoutRule::Void;
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
