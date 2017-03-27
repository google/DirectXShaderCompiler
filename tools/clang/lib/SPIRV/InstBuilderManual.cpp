//===-- InstBuilder.cpp - SPIR-V instruction builder ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/InstBuilder.h"

#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

void InstBuilder::encodeString(std::string value) {
  const size_t numChars = value.size();
  const size_t oldSize = TheInst.size();
  TheInst.resize(oldSize + (numChars / 4 + 1));
  // Make sure all the bytes in the last word are zeros, in case we only
  // write a partial word at the end.
  TheInst.back() = 0;
  // From the SPIR-V spec, literal string is
  //
  // A nul-terminated stream of characters consuming an integral number of
  // words. The character set is Unicode in the UTF-8 encoding scheme. The UTF-8
  // octets (8-bit bytes) are packed four per word, following the little-endian
  // convention (i.e., the first octet is in the lowest-order 8 bits of the
  // word). The final word contains the string's nul-termination character (0),
  // and all contents past the end of the string in the final word are padded
  // with 0.
  //
  // So the following works on little endian machines.
  char *strDest = reinterpret_cast<char *>(&TheInst[oldSize]);
  strncpy(strDest, value.c_str(), numChars);
};

} // end namespace spirv
} // end namespace clang