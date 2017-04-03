//===--- ModuleBuilder.cpp - SPIR-V builder implementation ----*- C++ -*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/ModuleBuilder.h"

#include "clang/SPIRV/spirv.hpp"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

namespace {
constexpr size_t kHeaderSize = 5;
constexpr size_t kBoundIndex = 3;

constexpr uint32_t kGeneratorNumber = 14;
constexpr uint32_t kToolVersion = 0;
}

ModuleBuilder::ModuleBuilder(SPIRVContext *C) : TheContext(*C) {}

void ModuleBuilder::BeginModule() { GenHeader(); }

void ModuleBuilder::EndModule() {
  assert(!TheModule.empty() && "BeginModule() not called before EndModule()");
  TheModule[kBoundIndex] = TheContext.getNextId();
}
std::vector<uint32_t> ModuleBuilder::TakeModule() {
  return std::move(TheModule);
}

void ModuleBuilder::GenHeader() {
  assert(TheModule.empty() && "Header not at the beginning");
  TheModule.reserve(kHeaderSize);
  TheModule.emplace_back(spv::MagicNumber);
  TheModule.emplace_back(spv::Version);
  TheModule.emplace_back((kGeneratorNumber << 16) | kToolVersion);
  TheModule.emplace_back(0u);  // Bound
  TheModule.emplace_back(0u);  // Schema
}

} // end namespace spirv
} // end namespace clang
