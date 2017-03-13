//===- unittests/SPIRV/DecorationTest.cpp ----- Decoration tests ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gmock/gmock.h"
#include "clang/SPIRV/Decoration.h"
#include "clang/SPIRV/SPIRVContext.h"
#include "gtest/gtest.h"

namespace {

TEST(ValidateType, ValidateSimpleDecorationUniqueness) {
  using namespace clang::spirv;
  SPIRVContext ctx;
  const Decoration *relaxed = Decoration::getDecorationRelaxedPrecision(ctx);
  const Decoration *relaxed_2 = Decoration::getDecorationRelaxedPrecision(ctx);
  EXPECT_EQ(relaxed, relaxed_2);
}

TEST(ValidateType, ValidateComplexDecorationUniqueness) {
  using namespace clang::spirv;
  SPIRVContext ctx;
  const Decoration *mem_0_builtin_pos =
      Decoration::getDecorationBuiltIn(ctx, spv::BuiltIn::Position, 0);
  const Decoration *mem_0_builtin_pos_again =
      Decoration::getDecorationBuiltIn(ctx, spv::BuiltIn::Position, 0);

  // We must get the same pointer for the same decoration signature.
  EXPECT_EQ(mem_0_builtin_pos, mem_0_builtin_pos_again);
}

TEST(ValidateType, ValidateMemberDecorationsDiffer) {
  using namespace clang::spirv;
  SPIRVContext ctx;
  // Applied to member 0 of a structure.
  const Decoration *mem_0_builtin_pos =
      Decoration::getDecorationBuiltIn(ctx, spv::BuiltIn::Position, 0);
  // Applied to member 1 of a structure.
  const Decoration *mem_1_builtin_pos =
      Decoration::getDecorationBuiltIn(ctx, spv::BuiltIn::Position, 1);

  // We should get different pointers for these decorations as they are
  // applied to different structure members.
  EXPECT_NE(mem_0_builtin_pos, mem_1_builtin_pos);
}

// TODO: Add decorations tests for all decorations

} // anonymous namespace
