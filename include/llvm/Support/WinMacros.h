#ifndef LLVM_SUPPORT_WINMACROS_H
#define LLVM_SUPPORT_WINMACROS_H

#ifndef _WIN32

#define C_ASSERT(expr) static_assert((expr), "")
#define ATLASSERT assert

#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

#define _countof(a) (sizeof(a) / sizeof(*(a)))

#define __declspec(x)

#define uuid(id)

#define STDMETHODCALLTYPE
#define STDAPI extern "C" HRESULT STDAPICALLTYPE
#define STDAPI_(type) extern "C" type STDAPICALLTYPE
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE

#define UNREFERENCED_PARAMETER(P) (void)(P)

#define RtlEqualMemory(Destination, Source, Length)                            \
  (!memcmp((Destination), (Source), (Length)))
#define RtlMoveMemory(Destination, Source, Length)                             \
  memmove((Destination), (Source), (Length))
#define RtlCopyMemory(Destination, Source, Length)                             \
  memcpy((Destination), (Source), (Length))
#define RtlFillMemory(Destination, Length, Fill)                               \
  memset((Destination), (Fill), (Length))
#define RtlZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory

#define FALSE 0
#define TRUE 1

#define REGDB_E_CLASSNOTREG 1

// We ignore the code page completely on Linux.
#define GetConsoleOutputCP() 0

#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#define DISP_E_BADINDEX _HRESULT_TYPEDEF_(0x8002000BL)

// TODO(ehsann): This is an unsafe conversion.
#define UIntToInt(uint_arg, int_ptr_arg) *int_ptr_arg = uint_arg

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

// Use errno to implement {Get|Set}LastError
#define GetLastError() errno
#define SetLastError(ERR) errno = ERR

// Map these errors to equivalent errnos.
#define ERROR_SUCCESS 0L
#define ERROR_OUT_OF_STRUCTURES ENOMEM
#define ERROR_UNHANDLED_EXCEPTION EINTR
#define ERROR_NOT_FOUND ENOTSUP
#define ERROR_NOT_CAPABLE EPERM
#define ERROR_FILE_NOT_FOUND ENOENT
#define ERROR_IO_DEVICE EIO
#define ERROR_INVALID_HANDLE EBADF

// Used by HRESULT <--> WIN32 error code conversion
#define SEVERITY_ERROR 1
#define FACILITY_WIN32 7
#define HRESULT_CODE(hr) ((hr) & 0xFFFF)
#define MAKE_HRESULT(severity, facility, code)                                 \
  ((HRESULT)(((unsigned long)(severity) << 31) |                               \
             ((unsigned long)(facility) << 16) | ((unsigned long)(code))))

#define FILE_TYPE_UNKNOWN 0x0000
#define FILE_TYPE_DISK 0x0001
#define FILE_TYPE_CHAR 0x0002
#define FILE_TYPE_PIPE 0x0003
#define FILE_TYPE_REMOTE 0x8000

#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// STGTY ENUMS
#define STGTY_STORAGE 1
#define STGTY_STREAM 2
#define STGTY_LOCKBYTES 3
#define STGTY_PROPERTY 4

// Storage errors
#define STG_E_INVALIDFUNCTION 1L
#define STG_E_ACCESSDENIED 2L

#define STREAM_SEEK_SET 0
#define STREAM_SEEK_CUR 1
#define STREAM_SEEK_END 2

#define HEAP_NO_SERIALIZE 1

#define MB_ERR_INVALID_CHARS 0x00000008 // error for invalid chars

#define _atoi64 atoll
#define sprintf_s snprintf
#define _strdup strdup
#define vsprintf_s vsprintf
#define strcat_s strcat

#define OutputDebugStringW(msg) fputws(msg, stderr)

#define OutputDebugStringA(msg) fputs(msg, stderr)
#define OutputDebugFormatA(...) fprintf(stderr, __VA_ARGS__)

// Event Tracing for Windows (ETW) provides application programmers the ability
// to start and stop event tracing sessions, instrument an application to
// provide trace events, and consume trace events.
#define DxcEtw_DXCompilerCreateInstance_Start()
#define DxcEtw_DXCompilerCreateInstance_Stop(hr)
#define DxcEtw_DXCompilerCompile_Start()
#define DxcEtw_DXCompilerCompile_Stop(hr)
#define DxcEtw_DXCompilerDisassemble_Start()
#define DxcEtw_DXCompilerDisassemble_Stop(hr)
#define DxcEtw_DXCompilerPreprocess_Start()
#define DxcEtw_DXCompilerPreprocess_Stop(hr)
#define DxcEtw_DxcValidation_Start()
#define DxcEtw_DxcValidation_Stop(hr)

#define UInt32Add UIntAdd
#define Int32ToUInt32 IntToUInt

#endif // _WIN32

#endif // LLVM_SUPPORT_WINMACROS_H
