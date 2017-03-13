//===- unittests/SPIRV/SPIRVContextTest.cpp ----- SPIRVContext tests ------===//
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
#include "clang/SPIRV/Type.h"
#include "gtest/gtest.h"

namespace {

TEST(ValidateSPIRVContext, ValidateGetNextId) {
  clang::spirv::SPIRVContext context;
  // Check that the first ID is 1.
  EXPECT_EQ(context.GetNextId(), 1u);
  // Check that calling GetNextId() multiple times does not increment the ID
  EXPECT_EQ(context.GetNextId(), 1u);
}

TEST(ValidateSPIRVContext, ValidateTakeNextId) {
  clang::spirv::SPIRVContext context;
  EXPECT_EQ(context.TakeNextId(), 1u);
  EXPECT_EQ(context.TakeNextId(), 2u);
  EXPECT_EQ(context.GetNextId(), 3u);
}

TEST(ValidateSPIRVContext, ValidateUniqueIdForUniqueNonAggregateType) {
  using namespace clang::spirv;
  clang::spirv::SPIRVContext ctx;
  auto NextIdFunc = [&ctx]() { return ctx.TakeNextId(); };
  const Type *intt = Type::getTypeInt32(ctx);
  uint32_t intt_id = ctx.GetResultIdForType(intt, NextIdFunc);
  uint32_t intt_id_again = ctx.GetResultIdForType(intt, NextIdFunc);
  // We should get the same ID for the same non-aggregate type.
  EXPECT_EQ(intt_id, intt_id_again);
}

TEST(ValidateSPIRVContext, ValidateUniqueIdForUniqueAggregateType) {
  using namespace clang::spirv;
  clang::spirv::SPIRVContext ctx;
  auto NextIdFunc = [&ctx]() { return ctx.TakeNextId(); };
  // In this test we construct a struct which includes an integer member and
  // a boolean member.
  // We also assign RelaxedPrecision decoration to the struct as a whole.
  // We also assign BufferBlock decoration to the struct as a whole.
  // We also assign Offset decoration to each member of the struct.
  // We also assign a BuiltIn decoration to the first member of the struct.
  const Type *intt = Type::getTypeInt32(ctx);
  const Type *boolt = Type::getTypeBool(ctx);
  const uint32_t intt_id = ctx.GetResultIdForType(intt, NextIdFunc);
  const uint32_t boolt_id = ctx.GetResultIdForType(boolt, NextIdFunc);
  const Decoration *relaxed = Decoration::getDecorationRelaxedPrecision(ctx);
  const Decoration *bufferblock = Decoration::getDecorationBufferBlock(ctx);
  const Decoration *mem_0_offset = Decoration::getDecorationOffset(ctx, 0u, 0);
  const Decoration *mem_1_offset = Decoration::getDecorationOffset(ctx, 0u, 1);
  const Decoration *mem_0_position =
      Decoration::getDecorationBuiltIn(ctx, spv::BuiltIn::Position, 0);

  const Type *struct_1 = Type::getType(
      ctx, spv::Op::OpTypeStruct, {intt_id, boolt_id},
      {relaxed, bufferblock, mem_0_offset, mem_1_offset, mem_0_position});

  const Type *struct_2 = Type::getType(
      ctx, spv::Op::OpTypeStruct, {intt_id, boolt_id},
      {relaxed, bufferblock, mem_0_offset, mem_1_offset, mem_0_position});

  const uint32_t struct_1_id = ctx.GetResultIdForType(struct_1, NextIdFunc);
  const uint32_t struct_2_id = ctx.GetResultIdForType(struct_2, NextIdFunc);

  // We should be able to retrieve the same ID for the same Type.
  EXPECT_EQ(struct_1_id, struct_2_id);
}

// TODO: Add more SPIRVContext tests

} // anonymous namespace
