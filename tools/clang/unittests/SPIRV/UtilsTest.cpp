//===- unittests/SPIRV/UtilsTest.cpp ------ Utils tests -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/Utils.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace {

using ::testing::ElementsAre;

TEST(ValidateUtils, ValidateStringToUintVecConversion) {
  // Bin  01110100   01110011    01100101    01010100 = unsigned(1,953,719,636)
  // Hex     74         73          65          54
  //          t          s           e           T
  // Bin  01101001   01110010    01110100    01010011 =  unsigned(1,769,108,563)
  // Hex     69         72          74          53
  //          i          r           t           S
  // Bin  00000000   00000000    01100111    01101110 =  unsigned(26,478)
  // Hex      0          0          67          6E
  //          \0         \0          g           n
  std::string str = "TestString";
  std::vector<uint32_t> words =
      clang::spirv::utils::reinterpretStringAsUintVec(str);
  EXPECT_THAT(words, ElementsAre(1953719636, 1769108563, 26478));
}
TEST(ValidateUtils, ValidateUintVecToStringConversion) {
  // Bin  01110100   01110011    01100101    01010100 = unsigned(1,953,719,636)
  // Hex     74         73          65          54
  //          t          s           e           T
  // Bin  01101001   01110010    01110100    01010011 =  unsigned(1,769,108,563)
  // Hex     69         72          74          53
  //          i          r           t           S
  // Bin  00000000   00000000    01100111    01101110 =  unsigned(26,478)
  // Hex      0          0          67          6E
  //          \0         \0          g           n
  std::vector<uint32_t> words = { 1953719636, 1769108563, 26478 };
  std::string str = clang::spirv::utils::reinterpretUintVecAsString(words);
  EXPECT_EQ(str, "TestString");
}
TEST(ValidateUtils, ValidateTwoWayUintVecToStringConversion) {
  std::string str = "TestString";
  // Convert to vector
  std::vector<uint32_t> words =
      clang::spirv::utils::reinterpretStringAsUintVec(str);

  // Convert back to string
  std::string result = clang::spirv::utils::reinterpretUintVecAsString(words);

  EXPECT_EQ(str, result);
}

// TODO: Add more ModuleBuilder tests

} // anonymous namespace
