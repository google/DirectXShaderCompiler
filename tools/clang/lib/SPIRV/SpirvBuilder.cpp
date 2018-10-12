//===--- SpirvBuilder.cpp - SPIR-V Builder Implementation --------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/SpirvBuilder.h"
#include "llvm/Support/MathExtras.h"

namespace clang {
namespace spirv {

SpirvBuilder::SpirvBuilder(SpirvContext &ctx)
    : context(ctx), module(nullptr), function(nullptr) {
  module = new (context) SpirvModule;
}

SpirvFunction *SpirvBuilder::beginFunction(QualType returnType,
                                           SourceLocation loc,
                                           llvm::StringRef funcName) {
  assert(!function && "found nested function");

  // Create the function
  function = new (context) SpirvFunction(
      returnType, /*id*/ 0, spv::FunctionControlMask::MaskNone, loc);

  // Add a debug name for the function
  auto *debugName = new (context) SpirvName(loc, function, funcName);
  module->addDebugName(debugName);

  return function;
}

SpirvFunctionParameter *SpirvBuilder::addFnParam(QualType ptrType,
                                                 SourceLocation loc,
                                                 llvm::StringRef name) {
  assert(function && "found detached parameter");

  // Create the function parameter
  auto *param = new (context) SpirvFunctionParameter(ptrType, /*id*/ 0, loc);
  function->addParameter(param);

  // Add a debug name for the parameter
  auto *debugName = new (context) SpirvName(loc, param, name);
  module->addDebugName(debugName);

  return param;
}

SpirvVariable *SpirvBuilder::addFnVar(QualType valueType, SourceLocation loc,
                                      llvm::StringRef name,
                                      SpirvInstruction *init) {
  assert(function && "found detached local variable");

  // Create the variable
  auto *var = new (context) SpirvVariable(valueType, /*id*/ 0, loc,
                                          spv::StorageClass::Function, init);
  function->addVariable(var);

  // Add a debug name for the variable
  auto *debugName = new (context) SpirvName(loc, var, name);
  module->addDebugName(debugName);

  return var;
}

void SpirvBuilder::endFunction() {
  assert(function && "no active function");

  // Move all basic blocks into the current function.
  // TODO: we should adjust the order the basic blocks according to
  // SPIR-V validation rules.
  for (auto *bb : basicBlocks) {
    function->addBasicBlock(bb);
  }
  basicBlocks.clear();

  module->addFunction(function);
  function = nullptr;
  insertPoint = nullptr;
}

SpirvBasicBlock *SpirvBuilder::createBasicBlock(llvm::StringRef name) {
  assert(function && "found detached basic block");

  // Create the basic block
  auto *bb = new (context) SpirvBasicBlock(/*id*/ 0, name);
  basicBlocks.push_back(bb);

  // Add a debug name for the basic block
  // A basic block may not necessarily correspond to a specific source location.
  auto *debugName = new (context) SpirvName(/*Source Location */{}, bb, name);
  module->addDebugName(debugName);

  return bb;
}

void SpirvBuilder::addSuccessor(SpirvBasicBlock *successorBB) {
  assert(insertPoint && "null insert point");
  insertPoint->addSuccessor(successorBB);
}

void SpirvBuilder::setMergeTarget(SpirvBasicBlock *mergeLabel) {
  assert(insertPoint && "null insert point");
  insertPoint->setMergeTarget(mergeLabel);
}

void SpirvBuilder::setContinueTarget(SpirvBasicBlock *continueLabel) {
  assert(insertPoint && "null insert point");
  insertPoint->setContinueTarget(continueLabel);
}

} // end namespace spirv
} // end namespace clang
