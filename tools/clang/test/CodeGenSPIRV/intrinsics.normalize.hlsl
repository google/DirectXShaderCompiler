// Run: %dxc -T vs_6_0 -E main

// According to HLSL reference:
// The 'normalize' function can only operate on floats and vector of floats.

// CHECK:      [[glsl:%\d+]] = OpExtInstImport "GLSL.std.450"

void main() {
  float3 result3;

// CHECK:      [[c:%\d+]] = OpLoad %v3float %c
// CHECK-NEXT: [[normalize_c:%\d+]] = OpExtInst %v3float [[glsl]] Normalize [[c]]
// CHECK-NEXT: OpStore %result3 [[normalize_c]]
  float3 c;
  result3 = normalize(c);
}
