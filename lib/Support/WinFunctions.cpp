//===-- WinFunctions.cpp - Windows Functions for other platforms --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines Windows-specific functions used in the codebase for
// non-Windows platforms.
//
//===----------------------------------------------------------------------===//


#ifndef _WIN32

#include "llvm/Support/WinFunctions.h"

HRESULT StringCchPrintfA(char *dst, size_t dstSize, const char* format, ...) {
  va_list args;
  va_start(args, format);
  // C++11 snprintf can return the size of the resulting string if it was to be constructed.
  size_t size = snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
  if(size > dstSize) {
    *dst = '\0';
  } else {
    snprintf(dst, size, format, args);
  }
  va_end(args);
  return S_OK;
}
HRESULT UIntAdd(UINT uAugend, UINT uAddend, UINT* puResult) {
  HRESULT hr;
  if ((uAugend + uAddend) >= uAugend) {
    *puResult = (uAugend + uAddend);
    hr = S_OK;
  } else {
    *puResult = 0xffffffff;
    hr = (HRESULT)1L;
  }
  return hr;
}
HRESULT IntToUInt(int in, UINT* out) {
  HRESULT hr;
  if (in >= 0) {
    *out = (UINT)in;
    hr = S_OK;
  } else {
    *out = 0xffffffff;
    hr = (HRESULT)1L;
  }
  return hr;
}
HRESULT UInt32Mult(UINT a, UINT b, UINT* out) {
  uint64_t result = (uint64_t)a * (uint64_t)b;
  if(result > uint64_t(UINT_MAX))
    return (HRESULT)1L;

  *out = (uint32_t)result;
  return S_OK;
}

int _stricmp(const char *str1, const char *str2) {
  size_t i = 0;
  for (; str1[i] && str2[i]; ++i) {
    int d = std::tolower(str1[i]) - std::tolower(str2[i]);
    if (d != 0)
      return d;
  }
  return str1[i] - str2[i];
}

HRESULT CoGetMalloc(DWORD dwMemContext, IMalloc **ppMalloc) {
  *ppMalloc = new IMalloc;
  (*ppMalloc)->AddRef();
  return S_OK;
}


#endif // _WIN32

