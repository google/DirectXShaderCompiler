#ifndef LLVM_SUPPORT_WINDTYPES_H
#define LLVM_SUPPORT_WINDTYPES_H

#ifndef LLVM_ON_WIN32

#include <atomic>
#include <vector>
#include <string>
#include <cstring>
#include <cwchar>
#include <cassert>
#include <climits>
#include <fstream>
#include <stdint.h>
#include <stddef.h>
#include <typeinfo>
#include <typeindex>
#include "llvm/Support/WinResults.h"


typedef unsigned char BYTE;
typedef unsigned char* LPBYTE;

typedef BYTE BOOLEAN;
typedef BOOLEAN *PBOOLEAN;

typedef bool BOOL;
typedef BOOL* LPBOOL;

typedef int INT;
typedef long LONG;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef DWORD *LPDWORD;

typedef uint32_t UINT32;
typedef uint64_t UINT64;

typedef signed char         INT8, *PINT8;
typedef signed int          INT32, *PINT32;


typedef size_t SIZE_T;
typedef const char* LPCSTR;
typedef const char* PCSTR;

typedef int errno_t;

typedef wchar_t WCHAR;
typedef wchar_t* LPWSTR;
typedef wchar_t* PWCHAR;
typedef const wchar_t* LPCWSTR;
typedef const wchar_t* PCWSTR;

typedef WCHAR OLECHAR;
typedef OLECHAR *BSTR;
typedef OLECHAR *LPOLESTR;
typedef char* LPSTR;

typedef void* LPVOID;
typedef const void *LPCVOID;

typedef std::nullptr_t nullptr_t;

typedef struct _GUID {
  DWORD Data1;
  WORD  Data2;
  WORD  Data3;
  BYTE  Data4[8];
} GUID;
typedef GUID CLSID;
typedef const GUID& REFGUID;
typedef std::type_index REFIID;
typedef const GUID& REFCLSID;

#define __uuidof(T) std::type_index(typeid(T))
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), reinterpret_cast<void**>(ppType)

#define interface struct

class CAllocator {
public:
  static void* Reallocate(void* p, size_t nBytes) throw() {
    return realloc(p, nBytes);
  }
  static void* Allocate(size_t nBytes) throw() {
    return malloc(nBytes);
  }
  static void Free(void* p) throw() {
    free(p);
  }
};

// Note: This implementation is inspired by:
// https://msdn.microsoft.com/en-us/library/office/cc839627.aspx
interface IUnknown {
  virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) = 0;
  virtual ULONG AddRef() {
    ++m_count;
    return m_count;
  }
  virtual ULONG Release() {
    --m_count;
    if(m_count == 0) {
      delete this;
    }
    return m_count;
  }
  virtual ~IUnknown() {}

  template <class Q>
  HRESULT QueryInterface(Q** pp) {
    return QueryInterface(__uuidof(Q), (void**)pp);
  }

  private:
  std::atomic<unsigned long> m_count;
};

struct INoMarshal : public IUnknown {
};

template <class T>
class CComPtrBase {
protected:
  CComPtrBase() throw() { p = nullptr; }
  CComPtrBase(T* lp) throw() {
    p = lp;
    if (p != nullptr)
      p->AddRef();
  }
  void Swap(CComPtrBase& other) {
    T* pTemp = p;
    p = other.p;
    other.p = pTemp;
  }
public:
  virtual ~CComPtrBase() throw() {
    if (p) {
      p->Release();
      p = nullptr;
    }
  }
  operator T*() const throw() { return p; }
  T& operator*() const { return *p; }
  T* operator->() const { return p; }
  T** operator&() throw() {
    assert(p==nullptr);
    return &p;
  }
  bool operator!() const throw() { return (p == nullptr); }
  bool operator<(T* pT) const throw() { return p < pT; }
  bool operator!=(T* pT) const { return !operator==(pT); }
  bool operator==(T* pT) const throw() { return p == pT; }

  // Release the interface and set to nullptr
  void Release() throw() {
    T* pTemp = p;
    if (pTemp) {
      p = nullptr;
      pTemp->Release();
    }
  }

  // Attach to an existing interface (does not AddRef)
  void Attach(T* p2) throw() {
    if (p) {
      ULONG ref = p->Release();
      (ref);
      // Attaching to the same object only works if duplicate references are being coalesced.  Otherwise
      // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
      assert(ref != 0 || p2 != p);
    }
    p = p2;
  }
    
  // Detach the interface (does not Release)
  T* Detach() throw() {
    T* pt = p;
    p = nullptr;
    return pt;
  }

  HRESULT CopyTo(T** ppT) throw() {
      assert(ppT != nullptr);
      if (ppT == nullptr)
          return E_POINTER;
      *ppT = p;
      if (p)
          p->AddRef();
      return S_OK;
  }

  template <class Q>
  HRESULT QueryInterface(Q** pp) const throw() {
    assert(pp!=nullptr);
    return p->QueryInterface(__uuidof(Q), (void**)pp);
  }

  T* p;
};

template<class T>
class CComPtr : public CComPtrBase<T> {
public:
  CComPtr() throw() {}
  CComPtr(T* lp) throw() : CComPtrBase<T>(lp) {}
  CComPtr(const CComPtr<T>& lp) throw() : CComPtrBase<T>(lp.p) {}
  T* operator=(T* lp) throw() {
    if(*this!=lp) {
        CComPtr(lp).Swap(*this);
    }
    return *this;
  }

  inline bool IsEqualObject(IUnknown* pOther) throw() {
    if (this->p == nullptr && pOther == nullptr)
      return true;	// They are both NULL objects
    
    if (this->p == nullptr || pOther == nullptr)
      return false;	// One is NULL the other is not
    
    CComPtr<IUnknown> punk1;
    CComPtr<IUnknown> punk2;
    this->p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
    pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
    return punk1 == punk2;
  }

  void ComPtrAssign(IUnknown** pp, IUnknown* lp, REFIID riid) {
    IUnknown* pTemp = *pp; // takes ownership
    if (lp == nullptr || FAILED(lp->QueryInterface(riid, (void**)pp)))
      *pp = nullptr;
    if (pTemp)
      pTemp->Release();
  }

  template <typename Q>
  T* operator=(const CComPtr<Q>& lp) throw() {
    if( !this->IsEqualObject(lp) ) {
      ComPtrAssign((IUnknown**)&this->p, lp, __uuidof(T));
    }
    return *this;
  }

  T* operator=(const CComPtr<T>& lp) throw() {
    if(*this!=lp) {
      CComPtr(lp).Swap(*this);
    }
    return *this;
  }

  CComPtr(CComPtr<T>&& lp) throw() : CComPtrBase<T>() {
    lp.Swap(*this);
  }

  T* operator=(CComPtr<T>&& lp) throw() {
    if (*this != lp) {
        CComPtr(static_cast<CComPtr&&>(lp)).Swap(*this);
    }
    return *this;
  }
};

// Based on what I see, CComHeapPtr is basically the same as CHeapPtr.
// It is inherited from it and doesn't seem to add anything...
// TODO: Verify
#define CComHeapPtr CHeapPtr

struct IMalloc : public IUnknown {
  virtual void *Alloc(size_t size) {
    return malloc(size);
  }
  virtual void *Realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
  }
  virtual void Free(void* ptr) {
    free(ptr);
  }
  virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) {
    assert(false && "QueryInterface not implemented for IMalloc.");
    return -1;
  }
};

#define CoGetMalloc(dwMemContext, ppMalloc) \
  (*ppMalloc = new IMalloc) && true

// Stuff...
typedef void *HANDLE;

#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE(HINSTANCE);
//typedef HINSTANCE HMODULE;
typedef void* HMODULE;

#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

#define HRESULT_FROM_WIN32(x) (HRESULT)(x) <= 0 ? (HRESULT)(x) : (HRESULT) (((x) & 0x0000FFFF) | (7 << 16) | 0x80000000)

#define CP_UTF8 65001 // UTF-8 translation.


typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR  cFileName[ 260 ]; // This is defined for windows in minwindef.h
    WCHAR  cAlternateFileName[ 14 ];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;


typedef union _LARGE_INTEGER {
    struct {
      DWORD LowPart;
      LONG HighPart;
    };
    LONGLONG QuadPart;
} LARGE_INTEGER;
 
typedef LARGE_INTEGER *PLARGE_INTEGER;
 
typedef union _ULARGE_INTEGER {
    struct {
      DWORD LowPart;
      DWORD HighPart;
    };
    ULONGLONG QuadPart;
} ULARGE_INTEGER;
 
typedef ULARGE_INTEGER *PULARGE_INTEGER;

typedef struct tagSTATSTG
{
  LPOLESTR pwcsName;
  DWORD type;
  ULARGE_INTEGER cbSize;
  FILETIME mtime;
  FILETIME ctime;
  FILETIME atime;
  DWORD grfMode;
  DWORD grfLocksSupported;
  CLSID clsid;
  DWORD grfStateBits;
  DWORD reserved;
} 	STATSTG;

enum tagSTATFLAG {
  STATFLAG_DEFAULT = 0,
  STATFLAG_NONAME  = 1,
  STATFLAG_NOOPEN  = 2
};

struct ISequentialStream : public IUnknown
{
public:
  virtual HRESULT Read(void *pv, ULONG cb, ULONG *pcbRead) = 0;
  virtual HRESULT Write(const void *pv, ULONG cb, ULONG *pcbWritten) = 0;
};

struct IStream : public ISequentialStream
{
public:
    virtual HRESULT Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) = 0;
    virtual HRESULT SetSize(ULARGE_INTEGER libNewSize) = 0;
    virtual HRESULT CopyTo( 
        IStream *pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead,
        ULARGE_INTEGER *pcbWritten) = 0;
    
    virtual HRESULT Commit(DWORD grfCommitFlags) = 0;
    
    virtual HRESULT Revert(void) = 0;
    
    virtual HRESULT LockRegion( 
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) = 0;
    
    virtual HRESULT UnlockRegion( 
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) = 0;
    
    virtual HRESULT Stat( 
        STATSTG *pstatstg,
        DWORD grfStatFlag) = 0;
    
    virtual HRESULT Clone(IStream **ppstm) = 0;
};

template<class T>
class CSimpleArray : public std::vector<T> {
public:
  bool Add(const T &t) { this->push_back(t); return true; }
  int GetSize() { return this->size(); }
  T* GetData() { return this->data(); }
};

#define IsEqualGUI(a,b) !memcmp(&a, &b, sizeof(GUID))
//#define IsEqualIID(a, b) !memcmp(&a, &b, sizeof(GUID))
#define IsEqualIID(a, b) a==b
#define IsEqualCLSID(a, b) !memcmp(&a, &b, sizeof(GUID))

template< int t_nBufferLength = 128 >
class CW2AEX
{
  public:
  CW2AEX( LPCWSTR psz , UINT nCodePage = CP_UTF8)
  {
    if (nCodePage != CP_UTF8) {
      // you have reached the limits of this half-assed implementation. So sorry.
      m_psz = NULL;
      return;
    }

    if (!psz) {
      m_psz = NULL;
      return;
    }

    int len = wcslen(psz)*4;
    m_psz = new char[len];
    std::wcstombs(m_psz, psz, len);
  }

  ~CW2AEX() { delete [] m_psz; }

  operator LPSTR() const { return m_psz; }

  char *m_psz;
  char m_szBuffer[t_nBufferLength]; // dunno what this is for

};
typedef CW2AEX<> CW2A;

template< int t_nBufferLength = 128 >
class CA2WEX
{
  public:
  CA2WEX( LPCSTR psz, UINT nCodePage = CP_UTF8 )
  {
    if (nCodePage != CP_UTF8) {
      // you have reached the limits of this half-assed implementation. So sorry.
      m_psz = NULL;
      return;
    }

    if (!psz) {
      m_psz = NULL;
      return;
    }

    int len = strlen(psz) + 1;
    m_psz = new wchar_t[len];
    std::mbstowcs(m_psz, psz, len);
  }

  ~CA2WEX() { delete [] m_psz; }

  operator LPWSTR() const { return m_psz; }

  wchar_t *m_psz;
  wchar_t m_szBuffer[t_nBufferLength];

};
typedef CA2WEX<> CA2W;

template <class T, class Allocator = CAllocator>
class CHeapPtrBase
{
protected:
  CHeapPtrBase() throw() : m_pData(NULL) {}
  CHeapPtrBase(CHeapPtrBase<T, Allocator>& p) throw() {
    m_pData = p.Detach();  // Transfer ownership
  }
  explicit CHeapPtrBase(T* pData) throw() : m_pData(pData) {}
 
public:
  ~CHeapPtrBase() throw() {
    Free();
  }
 
protected:
  CHeapPtrBase<T, Allocator>& operator=(CHeapPtrBase<T, Allocator>& p) throw() {
    if(m_pData != p.m_pData)
      Attach(p.Detach());  // Transfer ownership
    return *this;
	}
public:
  operator T*() const throw() { return m_pData; }
  T* operator->() const throw() {
    assert(m_pData != NULL);
    return m_pData;
	}
 
  T** operator&() throw() {
    assert(m_pData == NULL);
    return &m_pData;
  }

  // Allocate a buffer with the given number of bytes
  bool AllocateBytes(size_t nBytes) throw() {
    assert(m_pData == NULL);
    m_pData = static_cast<T*>(malloc(nBytes * sizeof(char)));
    if (m_pData == NULL)
      return false;

    return true;
	}
 
	// Attach to an existing pointer (takes ownership)
  void Attach(T* pData) throw() {
    free(m_pData);
    m_pData = pData;
	}
 
  // Detach the pointer (releases ownership)
  T* Detach() throw() {
    T* pTemp = m_pData;
    m_pData = NULL;
    return pTemp;
  }
 
	// Free the memory pointed to, and set the pointer to NULL
  void Free() throw() {
    free(m_pData);
    m_pData = NULL;
  }
 
  // Reallocate the buffer to hold a given number of bytes
  bool ReallocateBytes(size_t nBytes) throw() {
    T* pNew;
    pNew = static_cast<T*>(realloc(m_pData, nBytes * sizeof(char)));
    if (pNew == NULL)
    	return false;
    m_pData = pNew;
    
    return true;
  }
 
public:
	T* m_pData;
};
 
template <typename T, class Allocator = CAllocator>
class CHeapPtr : public CHeapPtrBase<T, Allocator> {
public:
	CHeapPtr() throw() {}
  CHeapPtr(CHeapPtr<T, Allocator>& p) throw() : CHeapPtrBase<T, Allocator>(p) {}
  explicit CHeapPtr(T* p) throw() : CHeapPtrBase<T, Allocator>(p) {}
  CHeapPtr<T>& operator=(CHeapPtr<T, Allocator>& p) throw() {
    CHeapPtrBase<T, Allocator>::operator=(p);
    return *this;
  }

  // Allocate a buffer with the given number of elements
  bool Allocate(size_t nElements = 1) throw() {
    size_t nBytes= nElements * sizeof(T);
    return this->AllocateBytes(nBytes);
  }

  // Reallocate the buffer to hold a given number of elements
  bool Reallocate(size_t nElements) throw() {
    size_t nBytes= nElements * sizeof(T);
    return this->ReallocateBytes(nBytes);
  }
};


static HRESULT StringCchPrintfA(char *dst, size_t dstSize, const char* format, ...) {
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
static HRESULT UIntAdd(UINT uAugend, UINT uAddend, UINT* puResult) {
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
static HRESULT IntToUInt(int in, UINT* out) {
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
static HRESULT UInt32Mult(UINT a, UINT b, UINT* out) {
  uint64_t result = (uint64_t)a * (uint64_t)b;
  if(result > uint64_t(UINT_MAX))
    return (HRESULT)1L;

  *out = (uint32_t)result;
  return S_OK;
}
static int _stricmp(const char* str1, const char* str2) {
  size_t i = 0;
  for (; str1[i] && str2[i]; ++i) {
    int d = std::tolower(str1[i]) - std::tolower(str2[i]);
    if (d != 0) return d;
  }
  return str1[i] - str2[i];
}

#endif // LLVM_ON_WIN32

#endif // LLVM_SUPPORT_WINTYPES_H
