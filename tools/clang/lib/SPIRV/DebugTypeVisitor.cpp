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
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Mangle.h"
#include "clang/SPIRV/SpirvBuilder.h"
#include "clang/SPIRV/SpirvModule.h"
#include "llvm/ADT/SmallSet.h"

namespace clang {
namespace spirv {

SpirvDebugInstruction *
DebugTypeVisitor::lowerToDebugTypeComposite(const StructType *type) {
  // DebugTypeComposite is already lowered by LowerTypeVisitor,
  // but it is not completely lowered.
  // We have to update member information including offset and size.
  auto *instr =
      dyn_cast<SpirvDebugTypeComposite>(spvContext.getDebugType(type));
  SpirvDebugType *actualType = nullptr;
  if (instr) {
    auto *tempType = instr->getTypeTemplate();
    if (tempType) {
      auto &tempParams = tempType->getParams();
      for (auto &t : tempParams) {
        t->setActualType(
            dyn_cast<SpirvDebugType>(lowerToDebugType(t->getSpirvType())));
      }
    }
  }
  // TODO: else emit error!
  assert(instr && "StructType was not lowered by LowerTypeVisitor");
  if (instr->getFullyLowered())
    return instr;

  uint32_t sizeInBits = 0;
  uint32_t offsetInBits = 0;
  auto &members = instr->getMembers();
  for (auto *member : members) {
    auto *debugMember = dyn_cast<SpirvDebugTypeMember>(member);
    if (!debugMember)
      continue;

    SpirvDebugType *memberTy =
        dyn_cast<SpirvDebugType>(lowerToDebugType(debugMember->getSpirvType()));
    debugMember->setType(memberTy);

    uint32_t memberSizeInBits = memberTy->getSizeInBits();
    uint32_t memberOffset = debugMember->getOffset();
    if (memberOffset == UINT32_MAX)
      memberOffset = offsetInBits;
    debugMember->updateOffsetAndSize(memberOffset, memberSizeInBits);

    offsetInBits = memberOffset + memberSizeInBits;
    if (sizeInBits < offsetInBits)
      sizeInBits = offsetInBits;
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
  case SpirvType::TK_Struct: {
    const auto *structType = dyn_cast<StructType>(spirvType);
    debugType = lowerToDebugTypeComposite(structType);
    break;
  }
  // TODO: Add DebugTypeComposite for class and union.
  // TODO: Add DebugTypeEnum.
  case SpirvType::TK_Array: {
    auto *arrType = dyn_cast<ArrayType>(spirvType);
    SpirvDebugInstruction *elemDebugType =
        lowerToDebugType(arrType->getElementType());
    debugType = spvContext.getDebugTypeArray(spirvType, elemDebugType,
                                             {arrType->getElementCount()});
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

  // TODO: When we emit all debug type completely, we should remove "Unknown"
  // type.
  if (!debugType) {
    debugType =
        spvContext.getDebugTypeBasic(nullptr, "Unknown", 0, 0 /*Unspecified*/);
  }

  debugType->setAstResultType(astContext.VoidTy);
  debugType->setResultType(context.getVoidType());
  debugType->setInstructionSet(spvBuilder.getOpenCLDebugInfoExtInstSet());
  return debugType;
}

bool DebugTypeVisitor::visitInstruction(SpirvInstruction *instr) {
  if (auto *debugInstr = dyn_cast<SpirvDebugInstruction>(instr)) {
    // Set the result type of debug instructions to OpTypeVoid.
    // According to the OpenCL.DebugInfo.100 spec, all debug instructions are
    // OpExtInst with result type of void.
    debugInstr->setAstResultType(astContext.VoidTy);
    debugInstr->setResultType(spvContext.getVoidType());
    debugInstr->setInstructionSet(spvBuilder.getOpenCLDebugInfoExtInstSet());

    // The following instructions are the only debug instructions that contain a
    // debug type:
    // DebugGlobalVariable
    // DebugLocalVariable
    // DebugFunction
    // DebugFunctionDeclaration
    // TODO: We currently don't have a SpirvDebugFunctionDeclaration class. Add
    // one if needed.
    if (isa<SpirvDebugGlobalVariable>(debugInstr) ||
        isa<SpirvDebugLocalVariable>(debugInstr)) {
      const SpirvType *spirvType = debugInstr->getDebugSpirvType();
      if (spirvType) {
        SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
        if (auto *var = dyn_cast<SpirvDebugGlobalVariable>(debugInstr)) {
          var->setDebugType(debugType);
        } else if (auto *var = dyn_cast<SpirvDebugLocalVariable>(debugInstr)) {
          var->setDebugType(debugType);
        } else {
          llvm_unreachable("Debug instruction must be DebugGlobalVariable or "
                           "DebugLocalVariable");
        }
      }
    }
    if (auto *debugFunction = dyn_cast<SpirvDebugFunction>(debugInstr)) {
      const SpirvType *spirvType =
          debugFunction->getSpirvFunction()->getFunctionType();
      if (spirvType) {
        SpirvDebugInstruction *debugType = lowerToDebugType(spirvType);
        debugFunction->setDebugType(debugType);
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
    for (const auto typePair : spvContext.getDebugTypes()) {
      module->addDebugInfo(typePair.second);
    }
    for (auto *type : spvContext.getTailDebugTypes()) {
      module->addDebugInfo(type);
    }
  }

  return true;
}

} // namespace spirv
} // namespace clang
