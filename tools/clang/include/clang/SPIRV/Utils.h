//===-- Utils.h - SPIR-V Utils --------*- C++-*----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_UTILS_H
#define LLVM_CLANG_SPIRV_UTILS_H

#include <string>
#include <vector>

namespace clang {
namespace spirv {
namespace utils {

/// \brief Reinterprets a given string as sequence of words.
/// Assumes Little Endian architecture.
std::vector<uint32_t> reinterpretStringAsUintVec(std::string s);
  
/// \brief Reinterprets the given vector of 32-bit words as a string.
/// Expectes that the words represent a NULL-terminated string.
/// Assumes Little Endian architecture.
std::string reinterpretUintVecAsString(std::vector<uint32_t>& vec);

} // end namespace utils
} // end namespace spirv
} // end namespace clang

#endif