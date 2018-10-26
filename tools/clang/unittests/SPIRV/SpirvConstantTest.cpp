//===- unittests/SPIRV/SpirvConstantTest.cpp --- SPIR-V Constant tests ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "clang/SPIRV/SPIRVContext.h"
#include "clang/SPIRV/SpirvInstruction.h"

namespace {
using namespace clang::spirv;

TEST(SpirvConstant, Float16) {
  SpirvContext ctx;
  const uint16_t f16 = 12;
  SpirvConstantFloat constant(ctx.getFloatType(16), f16);
  EXPECT_EQ(f16, constant.getValue16());
}

TEST(SpirvConstant, Float32) {
  SpirvContext ctx;
  const float f32 = 1.5;
  SpirvConstantFloat constant(ctx.getFloatType(32), f32);
  EXPECT_EQ(f32, constant.getValue32());
}

TEST(SpirvConstant, Float64) {
  SpirvContext ctx;
  const double f64 = 3.14;
  SpirvConstantFloat constant(ctx.getFloatType(64), f64);
  EXPECT_EQ(f64, constant.getValue64());
}

} // anonymous namespace
