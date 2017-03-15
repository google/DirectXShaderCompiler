//===-- Context.h - Context holding SPIR-V codegen data ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_CONTEXT_H
#define LLVM_CLANG_SPIRV_CONTEXT_H

#include "clang/Frontend/FrontendAction.h"

namespace clang {
namespace spirv {

/// \brief A class for holding various data needed in SPIR-V codegen.
/// It should outlive all SPIR-V codegen components that requires/allocates
/// data.
class Context {
public:
  inline Context();

  // Disable copy/move (assignment) constructors.
  Context(const Context &) = delete;
  Context(Context &&) = delete;
  Context &operator=(const Context &) = delete;
  Context &operator=(Context &&) = delete;

  inline uint32_t GetNextId() const;
  inline uint32_t TakeNextId();

private:
  uint32_t NextId;
};

Context::Context() : NextId(1) {}
uint32_t Context::GetNextId() const { return NextId; }
uint32_t Context::TakeNextId() { return NextId++; }

} // end namespace spirv
} // end namespace clang

#endif