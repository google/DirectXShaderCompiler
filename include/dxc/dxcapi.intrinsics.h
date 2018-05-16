#ifndef __DXC_API_INTRINSICS__
#define __DXC_API_INTRINSICS__

// SPIRV change starts
#include "llvm/Support/WinTypes.h"
#include "llvm/Support/WinMacros.h"
#include "llvm/Support/WinSAL.h"
// SPIRV change ends

///////////////////////////////////////////////////////////////////////////////
// Intrinsic definitions.
#define AR_QUAL_IN             0x0000000000000010ULL
#define AR_QUAL_OUT            0x0000000000000020ULL
#define AR_QUAL_CONST          0x0000000000000200ULL
#define AR_QUAL_ROWMAJOR       0x0000000000000400ULL
#define AR_QUAL_COLMAJOR       0x0000000000000800ULL

#define AR_QUAL_IN_OUT (AR_QUAL_IN | AR_QUAL_OUT)

static const BYTE INTRIN_TEMPLATE_FROM_TYPE = 0xff;
static const BYTE INTRIN_TEMPLATE_VARARGS = 0xfe;

// Use this enumeration to describe allowed templates (layouts) in intrinsics.
enum LEGAL_INTRINSIC_TEMPLATES {
  LITEMPLATE_VOID   = 0,  // No return type.
  LITEMPLATE_SCALAR = 1,  // Scalar types.
  LITEMPLATE_VECTOR = 2,  // Vector types (eg. float3).
  LITEMPLATE_MATRIX = 3,  // Matrix types (eg. float3x3).
  LITEMPLATE_ANY    = 4,  // Any one of scalar, vector or matrix types (but not object).
  LITEMPLATE_OBJECT = 5,  // Object types.

  LITEMPLATE_COUNT = 6
};

// INTRIN_COMPTYPE_FROM_TYPE_ELT0 is for object method intrinsics to indicate
// that the component type of the type is taken from the first subelement of the
// object's template type; see for example Texture2D.Gather
static const BYTE INTRIN_COMPTYPE_FROM_TYPE_ELT0 = 0xff;

enum LEGAL_INTRINSIC_COMPTYPES {
  LICOMPTYPE_VOID = 0,            // void, used for function returns
  LICOMPTYPE_BOOL = 1,            // bool
  LICOMPTYPE_INT = 2,             // i32, int-literal
  LICOMPTYPE_UINT = 3,            // u32, int-literal
  LICOMPTYPE_ANY_INT = 4,         // i32, u32, i64, u64, int-literal
  LICOMPTYPE_ANY_INT32 = 5,       // i32, u32, int-literal
  LICOMPTYPE_UINT_ONLY = 6,       // u32, u64, int-literal; no casts allowed
  LICOMPTYPE_FLOAT = 7,           // f32, partial-precision-f32, float-literal
  LICOMPTYPE_ANY_FLOAT = 8,       // f32, partial-precision-f32, f64, float-literal, min10-float, min16-float, half
  LICOMPTYPE_FLOAT_LIKE = 9,      // f32, partial-precision-f32, float-literal, min10-float, min16-float, half
  LICOMPTYPE_FLOAT_DOUBLE = 10,   // f32, partial-precision-f32, f64, float-literal
  LICOMPTYPE_DOUBLE = 11,         // f64, float-literal
  LICOMPTYPE_DOUBLE_ONLY = 12,    // f64; no casts allowed
  LICOMPTYPE_NUMERIC = 13,        // float-literal, f32, partial-precision-f32, f64, min10-float, min16-float, int-literal, i32, u32, min12-int, min16-int, min16-uint, i64, u64
  LICOMPTYPE_NUMERIC32 = 14,      // float-literal, f32, partial-precision-f32, int-literal, i32, u32
  LICOMPTYPE_NUMERIC32_ONLY = 15, // float-literal, f32, partial-precision-f32, int-literal, i32, u32; no casts allowed
  LICOMPTYPE_ANY = 16,            // float-literal, f32, partial-precision-f32, f64, min10-float, min16-float, int-literal, i32, u32, min12-int, min16-int, min16-uint, bool, i64, u64
  LICOMPTYPE_SAMPLER1D = 17,
  LICOMPTYPE_SAMPLER2D = 18,
  LICOMPTYPE_SAMPLER3D = 19,
  LICOMPTYPE_SAMPLERCUBE = 20,
  LICOMPTYPE_SAMPLERCMP = 21,
  LICOMPTYPE_SAMPLER = 22,
  LICOMPTYPE_STRING = 23,
  LICOMPTYPE_WAVE = 24,
  LICOMPTYPE_UINT64 = 25,         // u64, int-literal
  LICOMPTYPE_FLOAT16 = 26,
  LICOMPTYPE_INT16 = 27,
  LICOMPTYPE_UINT16 = 28,
  LICOMPTYPE_NUMERIC16_ONLY = 29,

  LICOMPTYPE_COUNT = 30
};

static const BYTE IA_SPECIAL_BASE = 0xf0;
static const BYTE IA_R = 0xf0;
static const BYTE IA_C = 0xf1;
static const BYTE IA_R2 = 0xf2;
static const BYTE IA_C2 = 0xf3;
static const BYTE IA_SPECIAL_SLOTS = 4;

struct HLSL_INTRINSIC_ARGUMENT {
  LPCSTR pName;               // Name of the argument; the first argument has the function name.
  UINT64 qwUsage;             // A combination of AR_QUAL_IN|AR_QUAL_OUT|AR_QUAL_COLMAJOR|AR_QUAL_ROWMAJOR in parameter tables; other values possible elsewhere.

  BYTE uTemplateId;           // One of INTRIN_TEMPLATE_FROM_TYPE, INTRIN_TEMPLATE_VARARGS or the argument # the template (layout) must match (trivially itself).
  BYTE uLegalTemplates;       // A LEGAL_INTRINSIC_TEMPLATES value for allowed templates.
  BYTE uComponentTypeId;      // INTRIN_COMPTYPE_FROM_TYPE_ELT0, or the argument # the component (element type) must match (trivially itself).
  BYTE uLegalComponentTypes;  // A LEGAL_INTRINSIC_COMPTYPES value for allowed components.

  BYTE uRows;                 // Required number of rows, or one of IA_R/IA_C/IA_R2/IA_C2 for matching input constraints.
  BYTE uCols;                 // Required number of cols, or one of IA_R/IA_C/IA_R2/IA_C2 for matching input constraints.
};

struct HLSL_INTRINSIC {
  UINT Op;                              // Intrinsic Op ID
  BOOL bReadOnly;                       // Only read memory
  BOOL bReadNone;                       // Not read memory
  INT  iOverloadParamIndex;             // Parameter decide the overload type, -1 means ret type
  UINT uNumArgs;                        // Count of arguments in pArgs.
  const HLSL_INTRINSIC_ARGUMENT* pArgs; // Pointer to first argument.
};

#endif // __DXC_API_INTRINSICS__
