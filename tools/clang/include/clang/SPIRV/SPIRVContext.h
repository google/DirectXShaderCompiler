//===-- SPIRVContext.h - Context holding SPIR-V codegen data ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_SPIRVCONTEXT_H
#define LLVM_CLANG_SPIRV_SPIRVCONTEXT_H

#include "clang/Frontend/FrontendAction.h"

namespace clang {
namespace spirv {

/// \brief A class for holding various data needed in SPIR-V codegen.
/// It should outlive all SPIR-V codegen components that requires/allocates
/// data.
class SPIRVContext {
public:
  inline SPIRVContext();

  // Disable copy/move (assignment) constructors.
  SPIRVContext(const SPIRVContext &) = delete;
  SPIRVContext(SPIRVContext &&) = delete;
  SPIRVContext &operator=(const SPIRVContext &) = delete;
  SPIRVContext &operator=(SPIRVContext &&) = delete;

  inline uint32_t GetNextId() const;
  inline uint32_t TakeNextId();

private:
  uint32_t NextId;
};

SPIRVContext::SPIRVContext() : NextId(1) {}
uint32_t SPIRVContext::GetNextId() const { return NextId; }
uint32_t SPIRVContext::TakeNextId() { return NextId++; }

} // end namespace spirv
} // end namespace clang

#endif