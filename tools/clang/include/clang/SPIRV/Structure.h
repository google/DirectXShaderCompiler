//===--- Structure.h - SPIR-V representation structures ------*- C++ -*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines several classes for representing SPIR-V basic blocks,
// functions, and modules. They are not intended to be general representations
// that can be used for various purposes; instead, they are just special
// crafted to be provide structured representation of SPIR-V modules in the
// ModuleBuilder.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SPIRV_STRUCTURE_H
#define LLVM_CLANG_SPIRV_STRUCTURE_H

#include <memory>
#include <string>
#include <vector>

#include "spirv/1.0/spirv.hpp11"
#include "clang/SPIRV/InstBuilder.h"
#include "clang/SPIRV/Type.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/Optional.h"

namespace clang {
namespace spirv {

// TODO: do some statistics and switch to use SmallVector here if helps.
/// \brief The class representing a SPIR-V instruction.
using Instruction = std::vector<uint32_t>;

/// \brief The class representing a SPIR-V basic block.
class BasicBlock {
public:
  /// \brief Default constructs an empty basic block.
  inline BasicBlock();
  /// \brief Constructs a basic block with the given label id.
  inline explicit BasicBlock(uint32_t labelId);

  // Disable copy constructor/assignment until we find they are truly useful.
  BasicBlock(const BasicBlock &) = delete;
  BasicBlock &operator=(const BasicBlock &) = delete;
  // Allow move constructor/assignment since they are efficient.
  BasicBlock(BasicBlock &&that);
  BasicBlock &operator=(BasicBlock &&that);

  /// \brief Returns true if this basic block is empty, which has no <label-id>
  /// assigned and no instructions.
  inline bool isEmpty() const;
  /// \brief Clears all instructions in this basic block and turns this basic
  /// block into an empty basic block.
  inline void clear();

  /// \brief Serializes this basic block and feeds it to the comsumer in the
  /// given InstBuilder. After this call, this basic block will be in an empty
  /// state.
  void take(InstBuilder *builder);

  /// \brief add an instruction to this basic block.
  inline void addInstruction(Instruction &&);

private:
  uint32_t labelId; ///< The label id for this basic block. Zero means invalid.
  std::vector<Instruction> instructions;
};

/// \brief The class representing a SPIR-V function.
class Function {
public:
  /// \brief Default constructs an empty SPIR-V function.
  inline Function();
  /// \brief Constructs a SPIR-V function with the given parameters.
  inline Function(uint32_t resultType, uint32_t resultId,
                  spv::FunctionControlMask control, uint32_t functionType);

  // Disable copy constructor/assignment until we find they are truly useful.
  Function(const Function &) = delete;
  Function &operator=(const Function &) = delete;

  // Allow move constructor/assignment since they are efficient.
  Function(Function &&that);
  Function &operator=(Function &&that);

  /// \brief Returns true if this function is empty.
  inline bool isEmpty() const;
  /// \brief Clears all paramters and basic blocks and turns this function into
  /// an empty function.
  void clear();

  /// \brief Serializes this function and feeds it to the comsumer in the given
  /// InstBuilder. After this call, this function will be in an invalid state.
  void take(InstBuilder *builder);

  /// \brief Adds a parameter to this function.
  inline void addParameter(uint32_t paramResultType, uint32_t paramResultId);
  /// \brief Adds a basic block to this function.
  inline void addBasicBlock(std::unique_ptr<BasicBlock> block);

private:
  uint32_t resultType;
  uint32_t resultId;
  spv::FunctionControlMask funcControl;
  uint32_t funcType;
  /// Parameter <result-type> and <result-id> pairs.
  std::vector<std::pair<uint32_t, uint32_t>> parameters;
  std::vector<std::unique_ptr<BasicBlock>> blocks;
};

/// \brief The class representing a SPIR-V module.
class SPIRVModule {
public:
  /// \brief Default constructs an empty SPIR-V module.
  inline SPIRVModule();

  // Disable copy constructor/assignment until we find they are truly useful.
  SPIRVModule(const SPIRVModule &) = delete;
  SPIRVModule &operator=(const SPIRVModule &) = delete;

  // Allow move constructor/assignment since they are efficient.
  SPIRVModule(SPIRVModule &&that) = default;
  SPIRVModule &operator=(SPIRVModule &&that) = default;

  /// \brief Returns true if this module is empty.
  bool isEmpty() const;
  /// \brief Clears all instructions and functions and turns this module into
  /// an empty module.
  void clear();

  /// \brief Collects all the SPIR-V words in this module and consumes them
  /// using the consumer within the given InstBuilder. This method is
  /// destructive; the module will be consumed and cleared after calling it.
  void take(InstBuilder *builder);

  /// \brief Sets the id bound to the given bound.
  inline void setBound(uint32_t newBound);

  inline void addCapability(spv::Capability);
  inline void addExtension(std::string extension);
  inline void addExtInstSet(uint32_t setId, std::string extInstSet);
  inline void setAddressingModel(spv::AddressingModel);
  inline void setMemoryModel(spv::MemoryModel);
  inline void addEntryPoint(spv::ExecutionModel, uint32_t targetId,
                            std::string targetName,
                            std::initializer_list<uint32_t> intefaces);
  inline void addExecutionMode(Instruction &&);
  // TODO: source code debug information
  inline void addDebugName(uint32_t targetId,
                           llvm::Optional<uint32_t> memberIndex,
                           std::string name);
  inline void addDecoration(const Decoration &decoration, uint32_t targetId);
  inline void addType(const Type *type, uint32_t resultId);
  inline void addConstant(const Type &type, Instruction &&constant);
  // TODO: global variables
  inline void addFunction(std::unique_ptr<Function>);

private:
  /// \brief The struct representing a SPIR-V module header.
  struct Header {
    /// \brief Default constructs a SPIR-V module header with id bound 0.
    Header();

    /// \brief Feeds the consumer with all the SPIR-V words for this header.
    void collect(const WordConsumer &consumer);

    const uint32_t magicNumber;
    const uint32_t version;
    const uint32_t generator;
    uint32_t bound;
    const uint32_t reserved;
  };

  /// \brief The struct representing an extended instruction set.
  struct ExtInstSet {
    inline ExtInstSet(uint32_t id, std::string name);

    const uint32_t resultId;
    const std::string setName;
  };

  /// \brief The struct representing an entry point.
  struct EntryPoint {
    inline EntryPoint(spv::ExecutionModel, uint32_t id, std::string name,
                      std::initializer_list<uint32_t> interface);

    const spv::ExecutionModel executionModel;
    const uint32_t targetId;
    const std::string targetName;
    const std::initializer_list<uint32_t> interfaces;
  };

  /// \brief The struct representing a debug name.
  struct DebugName {
    inline DebugName(uint32_t id, llvm::Optional<uint32_t> index,
                     std::string targetName);

    const uint32_t targetId;
    const llvm::Optional<uint32_t> memberIndex;
    const std::string name;
  };

  /// \brief The struct representing a deocoration and its target <result-id>.
  struct DecorationIdPair {
    inline DecorationIdPair(const Decoration &decor, uint32_t id);

    const Decoration &decoration;
    const uint32_t targetId;
  };

  /// \brief The struct representing a type and its <result-id>.
  struct TypeIdPair {
    inline TypeIdPair(const Type &ty, uint32_t id);

    const Type &type;
    const uint32_t resultId;
  };

  /// \brief The struct representing a constant and its type.
  struct Constant {
    inline Constant(const Type &ty, Instruction &&value);
    const Type &type;
    Instruction constant;
  };

  Header header; ///< SPIR-V module header.
  std::vector<spv::Capability> capabilities;
  std::vector<std::string> extensions;
  std::vector<ExtInstSet> extInstSets;
  // addressing and memory model must exist for a valid SPIR-V module.
  // We make them optional here just to provide extra flexibility of
  // the representation.
  llvm::Optional<spv::AddressingModel> addressingModel;
  llvm::Optional<spv::MemoryModel> memoryModel;
  std::vector<EntryPoint> entryPoints;
  std::vector<Instruction> executionModes;
  // TODO: source code debug information
  std::vector<DebugName> debugNames;
  std::vector<DecorationIdPair> decorations;
  // Note that types and constants are interdependent; Types like arrays have
  // <result-id>s for constants in their definition, and constants all have
  // their corresponding types. We store types and constants separately, but
  // they should be handled together.
  llvm::MapVector<const Type *, uint32_t> types;
  std::vector<Constant> constants;
  // TODO: global variables
  std::vector<std::unique_ptr<Function>> functions;
};

BasicBlock::BasicBlock() : labelId(0) {}
BasicBlock::BasicBlock(uint32_t id) : labelId(id) {}

bool BasicBlock::isEmpty() const {
  return labelId == 0 && instructions.empty();
}
void BasicBlock::clear() {
  labelId = 0;
  instructions.clear();
}

void BasicBlock::addInstruction(Instruction &&inst) {
  instructions.push_back(std::move(inst));
}

Function::Function()
    : resultType(0), resultId(0),
      funcControl(spv::FunctionControlMask::MaskNone), funcType(0) {}

Function::Function(uint32_t rType, uint32_t rId,
                   spv::FunctionControlMask control, uint32_t fType)
    : resultType(rType), resultId(rId), funcControl(control), funcType(fType) {}

bool Function::isEmpty() const {
  return resultType == 0 && resultId == 0 &&
         funcControl == spv::FunctionControlMask::MaskNone && funcType == 0 &&
         parameters.empty() && blocks.empty();
}

void Function::addParameter(uint32_t rType, uint32_t rId) {
  parameters.emplace_back(rType, rId);
}

void Function::addBasicBlock(std::unique_ptr<BasicBlock> block) {
  blocks.push_back(std::move(block));
}

SPIRVModule::SPIRVModule()
    : addressingModel(llvm::None), memoryModel(llvm::None) {}

void SPIRVModule::setBound(uint32_t newBound) { header.bound = newBound; }

void SPIRVModule::addCapability(spv::Capability cap) {
  capabilities.push_back(cap);
}
void SPIRVModule::addExtension(std::string ext) {
  extensions.push_back(std::move(ext));
}
void SPIRVModule::addExtInstSet(uint32_t setId, std::string extInstSet) {
  extInstSets.emplace_back(setId, extInstSet);
}
void SPIRVModule::setAddressingModel(spv::AddressingModel am) {
  addressingModel = llvm::Optional<spv::AddressingModel>(am);
}
void SPIRVModule::setMemoryModel(spv::MemoryModel mm) {
  memoryModel = llvm::Optional<spv::MemoryModel>(mm);
}
void SPIRVModule::addEntryPoint(spv::ExecutionModel em, uint32_t targetId,
                                std::string name,
                                std::initializer_list<uint32_t> interfaces) {
  entryPoints.emplace_back(em, targetId, std::move(name),
                           std::move(interfaces));
}
void SPIRVModule::addExecutionMode(Instruction &&execMode) {
  executionModes.push_back(std::move(execMode));
}
void SPIRVModule::addDebugName(uint32_t targetId,
                               llvm::Optional<uint32_t> memberIndex,
                               std::string name) {
  debugNames.emplace_back(targetId, memberIndex, std::move(name));
}
void SPIRVModule::addDecoration(const Decoration &decoration,
                                uint32_t targetId) {
  decorations.emplace_back(decoration, targetId);
}
void SPIRVModule::addType(const Type *type, uint32_t resultId) {
  types.insert(std::make_pair(type, resultId));
}
void SPIRVModule::addConstant(const Type &type, Instruction &&constant) {
  constants.emplace_back(type, std::move(constant));
};
void SPIRVModule::addFunction(std::unique_ptr<Function> f) {
  functions.push_back(std::move(f));
}

SPIRVModule::ExtInstSet::ExtInstSet(uint32_t id, std::string name)
    : resultId(id), setName(name) {}

SPIRVModule::EntryPoint::EntryPoint(spv::ExecutionModel em, uint32_t id,
                                    std::string name,
                                    std::initializer_list<uint32_t> interface)
    : executionModel(em), targetId(id), targetName(std::move(name)),
      interfaces(std::move(interface)) {}

SPIRVModule::DebugName::DebugName(uint32_t id, llvm::Optional<uint32_t> index,
                                  std::string targetName)
    : targetId(id), memberIndex(index), name(std::move(targetName)) {}

SPIRVModule::DecorationIdPair::DecorationIdPair(const Decoration &decor,
                                                uint32_t id)
    : decoration(decor), targetId(id) {}

SPIRVModule::TypeIdPair::TypeIdPair(const Type &ty, uint32_t id)
    : type(ty), resultId(id) {}

SPIRVModule::Constant::Constant(const Type &ty, Instruction &&value)
    : type(ty), constant(std::move(value)) {}

} // end namespace spirv
} // end namespace clang

#endif