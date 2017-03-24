//===- unittests/SPIRV/ModuleBuilderTest.cpp ------ ModuleBuilder tests ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/spirv.hpp"
#include "clang/SPIRV/ModuleBuilder.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace {

using ::testing::ElementsAre;

TEST(ValidateModuleBuilder, ValidateModuleWithHeaderOnly) {
  clang::spirv::SPIRVContext context;
  clang::spirv::ModuleBuilder builder(&context);
  builder.BeginModule();
  builder.EndModule();
  std::vector<uint32_t> spvModule = builder.TakeModule();
  // At the very least, running BeginModule() and EndModule() should
  // create the SPIR-V Header. The header is exactly 5 words long.
  EXPECT_EQ(spvModule.size(), 5u);
  EXPECT_THAT(spvModule, ElementsAre(spv::MagicNumber, spv::Version, ~0u, 1u, 0u));
}

// TODO: Add more ModuleBuilder tests

} // anonymous namespace
