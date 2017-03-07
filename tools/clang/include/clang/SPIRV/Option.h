//===-- Option.h - An optional value ----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SPIRV_OPTION_H
#define LLVM_CLANG_SPIRV_OPTION_H

#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

/// \brief Represents an optional value of type T
template <typename T> class Option {
public:
  static Option some(T Val) { return Option(T); }
  static Option none() { return Option(); }

  Option(const Option &) = default;
  Option(Option &&) = default;

  Option &operator=(const Option &) = default;
  Option &operator=(Option &&) = default;

  bool isSome() const { return IsSome; }
  bool isNone() const { return !IsSome; }

  const T &unwrap() const {
    assert(isSome());
    return Value;
  }
  operator const T &() const {
    assert(isSome());
    return Value;
  }

private:
  Option() : IsSome(false), Value({}) {}
  explicit Option(T Val) : IsSome(true), Value(Val) {}

  bool IsSome;
  T Value;
};

} // end namespace spirv
} // end namespace clang

#endif