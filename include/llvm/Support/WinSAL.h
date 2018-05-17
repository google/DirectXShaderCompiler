#ifndef LLVM_SUPPORT_WIN_SAL_H
#define LLVM_SUPPORT_WIN_SAL_H

#ifndef _WIN32

#define _In_
#define _In_z_
#define _In_opt_
#define _In_opt_count_(size)
#define _In_opt_z_
#define _In_reads_(size)
#define _In_reads_bytes_(size)
#define _In_reads_bytes_opt_(size)
#define _In_reads_opt_(size)
#define _In_reads_to_ptr_(ptr)
#define _In_count_(size)
#define _In_range_(lb,ub)
#define _In_bytecount_(size)
#define _In_NLS_string_(size)
#define __in_bcount(size)

#define _Out_
#define _Out_bytecap_(nbytes)
#define _Out_writes_to_opt_(a,b)
#define _Outptr_
#define _Outptr_opt_
#define _Outptr_opt_result_z_
#define _Out_opt_
#define _Out_writes_(size)
#define _Out_write_bytes_(size)
#define _Out_writes_z_(size)
#define _Out_writes_all_(size)
#define _Out_writes_bytes_(size)
#define _Outref_result_buffer_(size)
#define _Outptr_result_buffer_(size)
#define _Out_cap_(size)
#define _Out_cap_x_(size)
#define _Out_range_(lb,ub)
#define _Outptr_result_z_
#define _Outptr_result_buffer_maybenull_(ptr)
#define _Outptr_result_maybenull_
#define _Outptr_result_nullonfailure_

#define __out_ecount_part(a,b)

#define _Inout_
#define _Inout_z_
#define _Inout_opt_
#define _Inout_cap_(size)
#define _Inout_count_(size) 
#define _Inout_count_c_(size)
#define _Inout_opt_count_c_(size)
#define _Inout_bytecount_c_(size)
#define _Inout_opt_bytecount_c_(size)

#define _Ret_maybenull_
#define _Ret_notnull_
#define _Ret_opt_

#define _Use_decl_annotations_
#define _Analysis_assume_(expr)
#define _Analysis_assume_nullterminated_(x)
#define _Success_(expr)

#define __inexpressible_readableTo(size)
#define __inexpressible_writableTo(size)

#define _Printf_format_string_
#define _Null_terminated_
#define __fallthrough

#define _Field_size_(size)
#define _Field_size_full_(size)
#define _Field_size_opt_(size)
#define _Post_writable_byte_size_(size)
#define _Post_readable_byte_size_(size) 
#define __drv_allocatesMem(mem)

#define _COM_Outptr_
#define _COM_Outptr_opt_
#define _COM_Outptr_result_maybenull_

#define _Null_
#define _Notnull_
#define _Maybenull_

#define _Outptr_result_bytebuffer_(size)

#define __debugbreak()

// GCC produces erros on calling convention attributes.
#ifdef __GNUC__
#define __cdecl
#define __CRTDECL
#define __stdcall
#define __vectorcall
#define __thiscall
#define __fastcall
#define __clrcall
#endif

#endif // _WIN32

#endif // LLVM_SUPPORT_WIN_SAL_H
