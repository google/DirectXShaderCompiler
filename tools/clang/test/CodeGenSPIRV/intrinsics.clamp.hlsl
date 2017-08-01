// Run: %dxc -T vs_6_0 -E main

// CHECK:      [[glsl:%\d+]] = OpExtInstImport "GLSL.std.450"

void main() {
// All arguments are signed integers, therefore SClamp must be used.
// CHECK:      [[int_x:%\d+]] = OpLoad %int %int_x
// CHECK-NEXT: [[int_min:%\d+]] = OpLoad %int %int_min
// CHECK-NEXT: [[int_max:%\d+]] = OpLoad %int %int_max
// CHECK-NEXT: {{%\d+}} = OpExtInst %int [[glsl]] SClamp [[int_x]] [[int_min]] [[int_max]]
  int int_x, int_min, int_max;
  int int_result = clamp(int_x, int_min, int_max);


// Arguments are mix of signed integer and unsigned integer, therefore UClamp must be used.
// BitCasts must be performed for non-uint args.
  uint2 uint2_x;
  int2 int2_min, int2_max;
// CHECK: {{%\d+}} = OpBitcast %v2uint {{%\d+}}
// CHECK: {{%\d+}} = OpBitcast %v2uint {{%\d+}}
// CHECK: {{%\d+}} = OpExtInst %v2uint [[glsl]] UClamp {{%\d+}} {{%\d+}} {{%\d+}}
  uint2 uint2_result = clamp(uint2_x, int2_min, int2_max);


// Arguments are mix of signed integer, unsigned integer, and float.
// Therefore FClamp must be used. Conversion to float must occur for non-float args.
  float3 float3_x;
  uint3  uint3_min;
  int3   int3_max;
// CHECK: {{%\d+}} = OpConvertUToF %v3float {{%\d+}}
// CHECK: {{%\d+}} = OpConvertSToF %v3float {{%\d+}}
// CHECK: {{%\d+}} = OpExtInst %v3float [[glsl]] FClamp {{%\d+}} {{%\d+}} {{%\d+}}
  float3 float3_result = clamp(float3_x, uint3_min, int3_max);


// Arguments are mix of signed integer, unsigned integer, and float.
  float4x4 float4x4_x, float4x4_min, float4x4_max;
// CHECK:      [[mat_x:%\d+]] = OpLoad %mat4v4float %float4x4_x
// CHECK-NEXT: [[mat_min:%\d+]] = OpLoad %mat4v4float %float4x4_min
// CHECK-NEXT: [[mat_max:%\d+]] = OpLoad %mat4v4float %float4x4_max
// CHECK-NEXT: [[x_row0:%\d+]] = OpCompositeExtract %v4float [[mat_x]] 0
// CHECK-NEXT: [[min_row0:%\d+]] = OpCompositeExtract %v4float [[mat_min]] 0
// CHECK-NEXT: [[max_row0:%\d+]] = OpCompositeExtract %v4float [[mat_max]] 0
// CHECK-NEXT: [[clamp_row0:%\d+]] = OpExtInst %v4float [[glsl]] FClamp [[x_row0]] [[min_row0]] [[max_row0]]
// CHECK-NEXT: [[x_row1:%\d+]] = OpCompositeExtract %v4float [[mat_x]] 1
// CHECK-NEXT: [[min_row1:%\d+]] = OpCompositeExtract %v4float [[mat_min]] 1
// CHECK-NEXT: [[max_row1:%\d+]] = OpCompositeExtract %v4float [[mat_max]] 1
// CHECK-NEXT: [[clamp_row1:%\d+]] = OpExtInst %v4float [[glsl]] FClamp [[x_row1]] [[min_row1]] [[max_row1]]
// CHECK-NEXT: [[x_row2:%\d+]] = OpCompositeExtract %v4float [[mat_x]] 2
// CHECK-NEXT: [[min_row2:%\d+]] = OpCompositeExtract %v4float [[mat_min]] 2
// CHECK-NEXT: [[max_row2:%\d+]] = OpCompositeExtract %v4float [[mat_max]] 2
// CHECK-NEXT: [[clamp_row2:%\d+]] = OpExtInst %v4float [[glsl]] FClamp [[x_row2]] [[min_row2]] [[max_row2]]
// CHECK-NEXT: [[x_row3:%\d+]] = OpCompositeExtract %v4float [[mat_x]] 3
// CHECK-NEXT: [[min_row3:%\d+]] = OpCompositeExtract %v4float [[mat_min]] 3
// CHECK-NEXT: [[max_row3:%\d+]] = OpCompositeExtract %v4float [[mat_max]] 3
// CHECK-NEXT: [[clamp_row3:%\d+]] = OpExtInst %v4float [[glsl]] FClamp [[x_row3]] [[min_row3]] [[max_row3]]
// CHECK-NEXT: {{%\d+}} = OpCompositeConstruct %mat4v4float [[clamp_row0]] [[clamp_row1]] [[clamp_row2]] [[clamp_row3]]
  float4x4 float4x4_result = clamp(float4x4_x, float4x4_min, float4x4_max);
}
