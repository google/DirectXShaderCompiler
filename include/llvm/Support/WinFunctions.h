//===-- WinFunctions.h - Windows Functions for other platforms --*- C++ -*-===//
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

#ifndef LLVM_SUPPORT_WINFUNCTIONS_H
#define LLVM_SUPPORT_WINFUNCTIONS_H

#ifndef _WIN32

#include "dxc/Support/WinAdapter.h"

HRESULT StringCchPrintfA(char *dst, size_t dstSize, const char *format, ...);
HRESULT UIntAdd(UINT uAugend, UINT uAddend, UINT *puResult);
HRESULT IntToUInt(int in, UINT *out);
HRESULT UInt32Mult(UINT a, UINT b, UINT *out);
int _stricmp(const char *str1, const char *str2);
HRESULT CoGetMalloc(DWORD dwMemContext, IMalloc **ppMalloc);

#endif // _WIN32

#endif // LLVM_SUPPORT_WINFUNCTIONS_H
