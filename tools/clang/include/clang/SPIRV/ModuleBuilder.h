//===-- ModuleBuilder.h - SPIR-V builder ----------------------*- C++ -*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_MODULEBUILDER_H
#define LLVM_CLANG_SPIRV_MODULEBUILDER_H

#include <memory>

#include "clang/SPIRV/InstBuilder.h"
#include "clang/SPIRV/SPIRVContext.h"
#include "clang/SPIRV/Structure.h"
#include "llvm/ADT/MapVector.h"

namespace clang {
namespace spirv {

/// \brief SPIR-V module builder.
///
/// This class exports API for constructing SPIR-V binary interactively.
class ModuleBuilder {
public:
  /// \brief Constructs a ModuleBuilder with the given SPIR-V context.
  explicit ModuleBuilder(SPIRVContext *);

  /// \brief Begins building a SPIR-V function. At any time, there can only
  /// exist at most one function under building. Returns the <result-id> for the
  /// function on success. Returns zero on failure.
  uint32_t beginFunction(uint32_t funcType, uint32_t returnType);
  /// \brief Ends building of the current function. Returns true of success,
  /// false on failure.
  bool endFunction();

  /// \brief Creates a SPIR-V basic block. On success, returns the <label-id>
  /// for the basic block. On failure, returns zero.
  uint32_t bbCreate();
  /// \brief Ends building the SPIR-V basic block having the given <label-id>
  /// with OpReturn. Returns true on success, false on failure.
  bool bbReturn(uint32_t labelId);

  /// \brief Sets insertion point to the basic block with the given <label-id>.
  /// Returns true on success, false on failure.
  bool setInsertPoint(uint32_t labelId);

  /// \brief Takes the SPIR-V module under building.
  std::vector<uint32_t> takeModule();

private:
  using OrderedBasicBlockMap =
      llvm::MapVector<uint32_t, std::unique_ptr<BasicBlock>>;

  SPIRVContext &theContext;              ///< The SPIR-V context.
  SPIRVModule theModule;                 ///< The module under building.
  std::unique_ptr<Function> theFunction; ///< The function under building.
  OrderedBasicBlockMap basicBlocks;      ///< The basic blocks under building.
  BasicBlock *insertPoint;               ///< The current insertion point.
  std::vector<uint32_t> constructSite;   ///< InstBuilder construction site.
  InstBuilder instBuilder;
};

} // end namespace spirv
} // end namespace clang

#endif