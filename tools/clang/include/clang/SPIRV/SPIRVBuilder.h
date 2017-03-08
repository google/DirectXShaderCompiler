//===-- SPIRVBuilder.h - SPIR-V builder --*- C++ -*------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_SPIRVBUILDER_H
#define LLVM_CLANG_SPIRV_SPIRVBUILDER_H

#include <vector>

namespace clang {
namespace spirv {

class SPIRVBuilder {
public:
  SPIRVBuilder();

  /// \brief Consumes the next unused result id.
  inline uint32_t TakeNextID();

  /// \brief Begins building a SPIR-V module.
  void BeginModule();
  /// \brief Ends building the current module.
  void EndModule();
  /// \brief Takes the SPIR-V module under building.
  std::vector<uint32_t> TakeModule();

private:
  /// \brief Generates a header into the SPIR-V module under building.
  void GenHeader();

  /// \brief The next unused result id.
  uint32_t NextID;
  /// \brief The module under building.
  std::vector<uint32_t> TheModule;
};

uint32_t SPIRVBuilder::TakeNextID() { return NextID++; }

} // end namespace spirv
} // end namespace clang

#endif