//===--- SPIRVBuilder.cpp - SPIR-V builder implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/SPIRVBuilder.h"

#include "clang/SPIRV/spirv.hpp"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

namespace {
constexpr size_t kHeaderSize = 5;
constexpr size_t kBoundIndex = 3;
}

SPIRVBuilder::SPIRVBuilder(Context *C) : TheContext(*C) {}

void SPIRVBuilder::BeginModule() { GenHeader(); }

void SPIRVBuilder::EndModule() {
  assert(!TheModule.empty() && "BeginModule() not called before EndModule()");
  TheModule[kBoundIndex] = TheContext.GetNextId();
}
std::vector<uint32_t> SPIRVBuilder::TakeModule() {
  return std::move(TheModule);
}

void SPIRVBuilder::GenHeader() {
  assert(TheModule.empty() && "Header not at the beginning");
  TheModule.reserve(kHeaderSize);
  TheModule.emplace_back(spv::MagicNumber);
  TheModule.emplace_back(spv::Version);
  TheModule.emplace_back(~0u); // TODO: register a generator number
  TheModule.emplace_back(0u);  // Bound
  TheModule.emplace_back(0u);  // Schema
}

} // end namespace spirv
} // end namespace clang