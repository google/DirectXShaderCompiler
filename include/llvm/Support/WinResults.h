#ifndef LLVM_SUPPORT_WINRESULTS_H
#define LLVM_SUPPORT_WINRESULTS_H

// On Windows, the following result codes are defined
// in various Win-specific headers.
#ifndef _WIN32

typedef signed int HRESULT;

#define S_OK 0x00000000
#define E_ABORT 0x80004004
#define E_ACCESSDENIED 0x80070005
#define E_FAIL 0x80004005
#define E_HANDLE 0x80070006
#define E_INVALIDARG 0x80070057
#define E_NOINTERFACE 0x80004002
#define E_NOTIMPL 0x80004001
#define E_OUTOFMEMORY 0x8007000E
#define E_POINTER 0x80004003
#define E_UNEXPECTED 0x8000FFFF

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define DXC_FAILED(hr) (((HRESULT)(hr)) < 0)

#endif // Non-Win32

#endif // LLVM_SUPPORT_WINRESULTS_H
