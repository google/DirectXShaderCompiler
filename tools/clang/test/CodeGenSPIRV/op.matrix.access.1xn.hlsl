// Run: %dxc -T vs_6_0 -E main

void main() {
// CHECK-LABEL: %bb_entry = OpLabel

    float1x3 mat;
    float3 vec3;
    float2 vec2;
    float scalar;
    uint index;

    // 1 element (from lvalue)
// CHECK:      [[access0:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_2
// CHECK-NEXT: [[load0:%\d+]] = OpLoad %float [[access0]]
// CHECK-NEXT: OpStore %scalar [[load0]]
    scalar = mat._m02; // Used as rvalue
// CHECK-NEXT: [[load1:%\d+]] = OpLoad %float %scalar
// CHECK-NEXT: [[access1:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_1
// CHECK-NEXT: OpStore [[access1]] [[load1]]
    mat._12 = scalar; // Used as lvalue

    // > 1 elements (from lvalue)
// CHECK-NEXT: [[access2:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_0
// CHECK-NEXT: [[load2:%\d+]] = OpLoad %float [[access2]]
// CHECK-NEXT: [[access3:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_2
// CHECK-NEXT: [[load3:%\d+]] = OpLoad %float [[access3]]
// CHECK-NEXT: [[access4:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_1
// CHECK-NEXT: [[load4:%\d+]] = OpLoad %float [[access4]]
// CHECK-NEXT: [[cc0:%\d+]] = OpCompositeConstruct %v3float [[load2]] [[load3]] [[load4]]
// CHECK-NEXT: OpStore %vec3 [[cc0]]
    vec3 = mat._11_13_12; // Used as rvalue
// CHECK-NEXT: [[rhs0:%\d+]] = OpLoad %v2float %vec2
// CHECK-NEXT: [[ce0:%\d+]] = OpCompositeExtract %float [[rhs0]] 0
// CHECK-NEXT: [[access5:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_0
// CHECK-NEXT: OpStore [[access5]] [[ce0]]
// CHECK-NEXT: [[ce1:%\d+]] = OpCompositeExtract %float [[rhs0]] 1
// CHECK-NEXT: [[access6:%\d+]] = OpAccessChain %_ptr_Function_float %mat %int_2
// CHECK-NEXT: OpStore [[access6]] [[ce1]]
    mat._m00_m02 = vec2; // Used as lvalue

    // The following statements will trigger errors:
    //   invalid format for vector swizzle
    // scalar = (mat + mat)._m02;
    // vec2 = (mat * mat)._11_12;

    // One level indexing (from lvalue)
// CHECK-NEXT: [[load9:%\d+]] = OpLoad %v3float %mat
// CHECK-NEXT: OpStore %vec3 [[load9]]
    vec3 = mat[0]; // Used as rvalue

    // One level indexing (from lvalue)
// CHECK-NEXT: [[load10:%\d+]] = OpLoad %v3float %vec3
// CHECK-NEXT: OpStore %mat [[load10]]
    mat[index] = vec3; // Used as lvalue

    // Two level indexing (from lvalue)
// CHECK-NEXT: [[access9:%\d+]] = OpAccessChain %_ptr_Function_float %mat %uint_1
// CHECK-NEXT: [[load11:%\d+]] = OpLoad %float [[access9]]
// CHECK-NEXT: OpStore %scalar [[load11]]
    scalar = mat[index][1]; // Used as rvalue

    // Two level indexing (from lvalue)
// CHECK-NEXT: [[load12:%\d+]] = OpLoad %float %scalar
// CHECK-NEXT: [[index0:%\d+]] = OpLoad %uint %index
// CHECK-NEXT: [[access10:%\d+]] = OpAccessChain %_ptr_Function_float %mat [[index0]]
// CHECK-NEXT: OpStore [[access10]] [[load12]]
    mat[0][index] = scalar; // Used as lvalue
}
