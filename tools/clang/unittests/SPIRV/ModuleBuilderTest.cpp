//===- unittests/SPIRV/ModuleBuilderTest.cpp ------ ModuleBuilder tests ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "clang/SPIRV/ModuleBuilder.h"
#include "clang/SPIRV/spirv.hpp"

namespace {

using ::testing::ElementsAre;

TEST(ModuleBuilder, ValidateModuleWithHeaderOnly) {
  clang::spirv::SPIRVContext context;
  clang::spirv::ModuleBuilder builder(&context);
  builder.BeginModule();
  builder.EndModule();
  std::vector<uint32_t> spvModule = builder.TakeModule();
  // At the very least, running BeginModule() and EndModule() should
  // create the SPIR-V Header. The header is exactly 5 words long.
  EXPECT_EQ(spvModule.size(), 5u);
  EXPECT_THAT(spvModule,
              ElementsAre(spv::MagicNumber, spv::Version, 14u << 16, 1u, 0u));
}

// TODO: Add more ModuleBuilder tests

} // anonymous namespace
