//===- unittests/SPIRV/ModuleBuilderTest.cpp ------ ModuleBuilder tests ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/ModuleBuilder.h"
#include "gtest/gtest.h"

namespace {

TEST(ValidateModuleBuilder, ValidateModuleWithHeaderOnly) {
  clang::spirv::SPIRVContext context;
  clang::spirv::ModuleBuilder builder(&context);
  builder.BeginModule();
  builder.EndModule();
  std::vector<uint32_t> spvModule = builder.TakeModule();
  // At the very least, running BeginModule() and EndModule() should
  // create the SPIR-V Header. The header is exactly 5 words long.
  EXPECT_NE(spvModule.size(), 0u);
  EXPECT_EQ(spvModule.size(), 5u);
}

// TODO: Add more ModuleBuilder tests

} // anonymous namespace
