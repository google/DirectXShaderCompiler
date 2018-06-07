///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// dxcapi.internal.h                                                         //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Provides non-public declarations for the DirectX Compiler component.      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef __DXC_API_INTERNAL__
#define __DXC_API_INTERNAL__

#include "dxcapi.h"

///////////////////////////////////////////////////////////////////////////////
// Forward declarations.
typedef struct ITextFont ITextFont;
typedef struct IEnumSTATSTG IEnumSTATSTG;
typedef struct ID3D10Blob ID3D10Blob;

#include "dxcapi.intrinsics.h"

///////////////////////////////////////////////////////////////////////////////
// Interfaces.
struct __declspec(uuid("f0d4da3f-f863-4660-b8b4-dfd94ded6215"))
IDxcIntrinsicTable : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE GetTableName(_Outptr_ LPCSTR *pTableName) = 0;
  virtual HRESULT STDMETHODCALLTYPE LookupIntrinsic(
    LPCWSTR typeName, LPCWSTR functionName,
    const HLSL_INTRINSIC** pIntrinsic,
    _Inout_ UINT64* pLookupCookie) = 0;

  // Get the lowering strategy for an hlsl extension intrinsic.
  virtual HRESULT STDMETHODCALLTYPE GetLoweringStrategy(UINT opcode, LPCSTR *pStrategy) = 0;
  
  // Callback to support custom naming of hlsl extension intrinsic functions in dxil.
  // Return the empty string to get the default intrinsic name, which is the mangled
  // name of the high level intrinsic function.
  //
  // Overloaded intrinsics are supported by use of an overload place holder in the
  // name. The string "$o" in the name will be replaced by the return type of the
  // intrinsic.
  virtual HRESULT STDMETHODCALLTYPE GetIntrinsicName(UINT opcode, LPCSTR *pName) = 0;

  // Callback to support the 'dxil' lowering strategy.
  // Returns the dxil opcode that the intrinsic should use for lowering.
  virtual HRESULT STDMETHODCALLTYPE GetDxilOpCode(UINT opcode, UINT *pDxilOpcode) = 0;
};

struct __declspec(uuid("1d063e4f-515a-4d57-a12a-431f6a44cfb9"))
IDxcSemanticDefineValidator : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE GetSemanticDefineWarningsAndErrors(LPCSTR pName, LPCSTR pValue, IDxcBlobEncoding **ppWarningBlob, IDxcBlobEncoding **ppErrorBlob) = 0;
};

struct __declspec(uuid("282a56b4-3f56-4360-98c7-9ea04a752272"))
IDxcLangExtensions : public IUnknown
{
public:
  /// <summary>
  /// Registers the name of a preprocessor define that has semantic meaning
  /// and should be preserved for downstream consumers.
  /// </summary>
  virtual HRESULT STDMETHODCALLTYPE RegisterSemanticDefine(LPCWSTR name) = 0;
  /// <summary>Registers a name to exclude from semantic defines.</summary>
  virtual HRESULT STDMETHODCALLTYPE RegisterSemanticDefineExclusion(LPCWSTR name) = 0;
  /// <summary>Registers a definition for compilation.</summary>
  virtual HRESULT STDMETHODCALLTYPE RegisterDefine(LPCWSTR name) = 0;
  /// <summary>Registers a table of built-in intrinsics.</summary>
  virtual HRESULT STDMETHODCALLTYPE RegisterIntrinsicTable(_In_ IDxcIntrinsicTable* pTable) = 0;
  /// <summary>Sets an (optional) validator for parsed semantic defines.<summary>
  /// This provides a hook to check that the semantic defines present in the source
  /// contain valid data. One validator is used to validate all parsed semantic defines.
  virtual HRESULT STDMETHODCALLTYPE SetSemanticDefineValidator(_In_ IDxcSemanticDefineValidator* pValidator) = 0;
  /// <summary>Sets the name for the root metadata node used in DXIL to hold the semantic defines.</summary>
  virtual HRESULT STDMETHODCALLTYPE SetSemanticDefineMetaDataName(LPCSTR name) = 0;
};

struct __declspec(uuid("454b764f-3549-475b-958c-a7a6fcd05fbc"))
IDxcSystemAccess : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE EnumFiles(LPCWSTR fileName, IEnumSTATSTG** pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE OpenStorage(
    _In_      LPCWSTR lpFileName,
    _In_      DWORD dwDesiredAccess,
    _In_      DWORD dwShareMode,
    _In_      DWORD dwCreationDisposition,
    _In_      DWORD dwFlagsAndAttributes, IUnknown** pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE SetStorageTime(_In_ IUnknown* storage,
    _In_opt_  const FILETIME *lpCreationTime,
    _In_opt_  const FILETIME *lpLastAccessTime,
    _In_opt_  const FILETIME *lpLastWriteTime) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetFileInformationForStorage(_In_ IUnknown* storage, _Out_ LPBY_HANDLE_FILE_INFORMATION lpFileInformation) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetFileTypeForStorage(_In_ IUnknown* storage, _Out_ DWORD* fileType) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateHardLinkInStorage(_In_ LPCWSTR lpFileName, _In_ LPCWSTR lpExistingFileName) = 0;
  virtual HRESULT STDMETHODCALLTYPE MoveStorage(_In_ LPCWSTR lpExistingFileName, _In_opt_ LPCWSTR lpNewFileName, _In_ DWORD dwFlags) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetFileAttributesForStorage(_In_ LPCWSTR lpFileName, _Out_ DWORD* pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE DeleteStorage(_In_ LPCWSTR lpFileName) = 0;
  virtual HRESULT STDMETHODCALLTYPE RemoveDirectoryStorage(LPCWSTR lpFileName) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateDirectoryStorage(_In_ LPCWSTR lpPathName) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetCurrentDirectoryForStorage(DWORD nBufferLength, _Out_writes_(nBufferLength) LPWSTR lpBuffer, _Out_ DWORD* written) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetMainModuleFileNameW(DWORD nBufferLength, _Out_writes_(nBufferLength) LPWSTR lpBuffer, _Out_ DWORD* written) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetTempStoragePath(DWORD nBufferLength, _Out_writes_(nBufferLength) LPWSTR lpBuffer, _Out_ DWORD* written) = 0;
  virtual HRESULT STDMETHODCALLTYPE SupportsCreateSymbolicLink(_Out_ BOOL* pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateSymbolicLinkInStorage(_In_ LPCWSTR lpSymlinkFileName, _In_ LPCWSTR lpTargetFileName, DWORD dwFlags) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateStorageMapping(
    _In_      IUnknown* hFile,
    _In_      DWORD flProtect,
    _In_      DWORD dwMaximumSizeHigh,
    _In_      DWORD dwMaximumSizeLow,
    _Outptr_  IUnknown** pResult) = 0;
  virtual HRESULT MapViewOfFile(
    _In_  IUnknown* hFileMappingObject,
    _In_  DWORD dwDesiredAccess,
    _In_  DWORD dwFileOffsetHigh,
    _In_  DWORD dwFileOffsetLow,
    _In_  SIZE_T dwNumberOfBytesToMap,
    _Outptr_ ID3D10Blob** pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE OpenStdStorage(int standardFD, _Outptr_ IUnknown** pResult) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetStreamDisplay(_COM_Outptr_result_maybenull_ ITextFont** textFont, _Out_ unsigned* columnCount) = 0;
};

struct __declspec(uuid("e991ca8d-2045-413c-a8b8-788b2c06e14d"))
IDxcContainerEventsHandler : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE OnDxilContainerBuilt(_In_ IDxcBlob *pSource, _Out_ IDxcBlob **ppTarget) = 0;
};

struct __declspec(uuid("0cfc5058-342b-4ff2-83f7-04c12aad3d01"))
IDxcContainerEvent : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE RegisterDxilContainerEventHandler(IDxcContainerEventsHandler *pHandler, UINT64 *pCookie) = 0;
  virtual HRESULT STDMETHODCALLTYPE UnRegisterDxilContainerEventHandler(UINT64 cookie) = 0;
};
#endif
