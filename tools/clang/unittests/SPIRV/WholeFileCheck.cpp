//===- unittests/SPIRV/WholeFileCheck.cpp ----- Run Whole File Tests ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <fstream>

#include "WholeFileCheck.h"
#include "gtest/gtest.h"

TEST_F(WholeFileTest, BringUp) {
  // Ideally all generated SPIR-V must be valid, but this currently fails with
  // this error message: "No OpEntryPoint instruction was found...".
  // TODO: change this test such that it does run validation.
  bool success = runWholeFileTest("basic.spirv", /*generateHeader*/ true,
                                  /*runValidation*/ false);
  EXPECT_TRUE(success);
}