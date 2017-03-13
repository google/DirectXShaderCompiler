//===-- Utils.cpp - SPIR-V Utils --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/Utils.h"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {
namespace utils {

/// \brief Reinterprets a given string as sequence of words.
/// Assumes Little Endian architecture.
std::vector<uint32_t> reinterpretStringAsUintVec(std::string s) {
  std::vector<uint32_t> results;
  if (!s.empty()) {
    unsigned int num_bytes_needed = s.size() + 1;
    unsigned int num_words_needed = (num_bytes_needed / 4) + 1;
    std::string full_str(4 * num_words_needed, '\0');
    full_str.insert(full_str.begin(), s.begin(), s.end());
    for (unsigned int word_index = 0; word_index < num_words_needed;
         ++word_index) {
      int i = 4 * word_index;
      uint32_t word = 0;
      word |= full_str[i + 3] << 24;
      word |= full_str[i + 2] << 16;
      word |= full_str[i + 1] << 8;
      word |= full_str[i];
      results.push_back(word);
    }
  }
  return results;
}

/// \brief Reinterprets the given vector of 32-bit words as a string.
/// Expectes that the words represent a NULL-terminated string.
/// Assumes Little Endian architecture.
std::string reinterpretUintVecAsString(std::vector<uint32_t>& vec) {
  std::string result;
  if (!vec.empty()) {
    result = std::string(reinterpret_cast<const char*>(&vec[0]));
  }
  return result;
}

} // end namespace utils
} // end namespace spirv
} // end namespace clang
