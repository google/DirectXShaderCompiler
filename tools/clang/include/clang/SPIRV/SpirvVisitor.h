//===-- SpirvVisitor.h - SPIR-V Visitor -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_VISITOR_H
#define LLVM_CLANG_SPIRV_VISITOR_H

#include "clang/SPIRV/SpirvInstruction.h"

namespace clang {
namespace spirv {

class SpirvModule;
class SpirvFunction;
class SpirvBasicBlock;

/// \brief The base class for different SPIR-V visitor classes.
/// Each Visitor class serves a specific purpose and should override the
/// visit(*) methods accordingly in order to achieve its purpose.
class Visitor {
public:
  enum Phase {
    Init, //< Before starting the visit of the given construct
    Done, //< After finishing the visit of the given construct
  };

  // Virtual destructor
  virtual ~Visitor() = default;

  // Forbid copy construction and assignment
  Visitor(const Visitor &) = delete;
  Visitor &operator=(const Visitor &) = delete;

  // Forbid move construction and assignment
  Visitor(Visitor &&) = delete;
  Visitor &operator=(Visitor &&) = delete;

  // Visiting different SPIR-V constructs.
  virtual bool visit(SpirvModule *, Phase) {}
  virtual bool visit(SpirvFunction *, Phase) {}
  virtual bool visit(SpirvBasicBlock *, Phase) {}
  virtual bool visit(SpirvInstruction *) {}
  virtual bool visit(SpirvCapability *) {}
  virtual bool visit(SpirvExtension *) {}
  virtual bool visit(SpirvExtInstImport *) {}
  virtual bool visit(SpirvMemoryModel *) {}
  virtual bool visit(SpirvEntryPoint *) {}
  virtual bool visit(SpirvExecutionMode *) {}
  virtual bool visit(SpirvString *) {}
  virtual bool visit(SpirvSource *) {}
  virtual bool visit(SpirvName *) {}
  virtual bool visit(SpirvModuleProcessed *) {}
  virtual bool visit(SpirvDecoration *) {}
  virtual bool visit(SpirvVariable *) {}
  virtual bool visit(SpirvFunctionParameter *) {}
  virtual bool visit(SpirvLoopMerge *) {}
  virtual bool visit(SpirvSelectionMerge *) {}
  virtual bool visit(SpirvBranching *) {}
  virtual bool visit(SpirvBranch *) {}
  virtual bool visit(SpirvBranchConditional *) {}
  virtual bool visit(SpirvKill *) {}
  virtual bool visit(SpirvReturn *) {}
  virtual bool visit(SpirvSwitch *) {}
  virtual bool visit(SpirvUnreachable *) {}
  virtual bool visit(SpirvAccessChain *) {}
  virtual bool visit(SpirvAtomic *) {}
  virtual bool visit(SpirvBarrier *) {}
  virtual bool visit(SpirvBinaryOp *) {}
  virtual bool visit(SpirvBitFieldExtract *) {}
  virtual bool visit(SpirvBitFieldInsert *) {}
  virtual bool visit(SpirvComposite *) {}
  virtual bool visit(SpirvCompositeExtract *) {}
  virtual bool visit(SpirvExtInst *) {}
  virtual bool visit(SpirvFunctionCall *) {}
  virtual bool visit(SpirvNonUniformBinaryOp *) {}
  virtual bool visit(SpirvNonUniformElect *) {}
  virtual bool visit(SpirvNonUniformUnaryOp *) {}
  virtual bool visit(SpirvImageOp *) {}
  virtual bool visit(SpirvImageQuery *) {}
  virtual bool visit(SpirvImageSparseTexelsResident *) {}
  virtual bool visit(SpirvImageTexelPointer *) {}
  virtual bool visit(SpirvLoad *) {}
  virtual bool visit(SpirvSampledImage *) {}
  virtual bool visit(SpirvSelect *) {}
  virtual bool visit(SpirvSpecConstantBinaryOp *) {}
  virtual bool visit(SpirvSpecConstantUnaryOp *) {}
  virtual bool visit(SpirvStore *) {}
  virtual bool visit(SpirvUnaryOp *) {}
  virtual bool visit(SpirvVectorShuffle *) {}

protected:
  Visitor() = default;
};

/// \breif The visitor class that emits the SPIR-V words from the in-memory
/// representation.
class EmitVisitor : public Visitor {
public:
  EmitVisitor() : Visitor(), spirvBinary({}) {}

  // Visit different SPIR-V constructs for emitting.
  bool visit(SpirvModule *, Phase phase);
  bool visit(SpirvFunction *, Phase phase);
  bool visit(SpirvBasicBlock *, Phase phase);

  bool visit(SpirvCapability *);
  bool visit(SpirvExtension *);
  bool visit(SpirvExtInstImport *);
  bool visit(SpirvMemoryModel *);
  bool visit(SpirvEntryPoint *);
  bool visit(SpirvExecutionMode *);
  bool visit(SpirvString *);
  bool visit(SpirvSource *);
  bool visit(SpirvName *);
  bool visit(SpirvModuleProcessed *);
  bool visit(SpirvDecoration *);
  bool visit(SpirvVariable *);
  bool visit(SpirvFunctionParameter *);
  bool visit(SpirvLoopMerge *);
  bool visit(SpirvSelectionMerge *);
  bool visit(SpirvBranching *);
  bool visit(SpirvBranch *);
  bool visit(SpirvBranchConditional *);
  bool visit(SpirvKill *);
  bool visit(SpirvReturn *);
  bool visit(SpirvSwitch *);
  bool visit(SpirvUnreachable *);
  bool visit(SpirvAccessChain *);
  bool visit(SpirvAtomic *);
  bool visit(SpirvBarrier *);
  bool visit(SpirvBinaryOp *);
  bool visit(SpirvBitFieldExtract *);
  bool visit(SpirvBitFieldInsert *);
  bool visit(SpirvComposite *);
  bool visit(SpirvCompositeExtract *);
  bool visit(SpirvExtInst *);
  bool visit(SpirvFunctionCall *);
  bool visit(SpirvNonUniformBinaryOp *);
  bool visit(SpirvNonUniformElect *);
  bool visit(SpirvNonUniformUnaryOp *);
  bool visit(SpirvImageOp *);
  bool visit(SpirvImageQuery *);
  bool visit(SpirvImageSparseTexelsResident *);
  bool visit(SpirvImageTexelPointer *);
  bool visit(SpirvLoad *);
  bool visit(SpirvSampledImage *);
  bool visit(SpirvSelect *);
  bool visit(SpirvSpecConstantBinaryOp *);
  bool visit(SpirvSpecConstantUnaryOp *);
  bool visit(SpirvStore *);
  bool visit(SpirvUnaryOp *);
  bool visit(SpirvVectorShuffle *);

private:
  // Initiates the creation of a new instruction with the given Opcode.
  // Providing a size hint can slightly improve performance for instructions
  // that have fixed size by reserving the needed vector size.
  void initInstruction(spv::Op, uint32_t sizeHint = 0);

  // Finalizes the current instruction by encoding the instruction size into the
  // first word, and then appends the current instruction to the SPIR-V binary.
  void finalizeInstruction();

  // Encodes the given string into the current instruction that is being built.
  void encodeString(llvm::StringRef value);

  // Provides the next available <result-id>
  uint32_t getNextId() { return ++id; }

private:
  uint32_t id;
  std::vector<uint32_t> curInst;
  std::vector<uint32_t> spirvBinary;
};

} // namespace spirv
} // namespace clang

#endif // LLVM_CLANG_SPIRV_VISITOR_H
