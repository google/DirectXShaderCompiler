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

#include "clang/SPIRV/Context.h"

namespace clang {
namespace spirv {

class SPIRVBuilder {
public:
  explicit SPIRVBuilder(Context *);

  /// \brief Begins building a SPIR-V module.
  void BeginModule();
  /// \brief Ends building the current module.
  void EndModule();
  /// \brief Takes the SPIR-V module under building.
  std::vector<uint32_t> TakeModule();

private:
  /// \brief Generates a header into the SPIR-V module under building.
  void GenHeader();

  Context &TheContext;
  /// \brief The module under building.
  std::vector<uint32_t> TheModule;
};

} // end namespace spirv
} // end namespace clang

#endif