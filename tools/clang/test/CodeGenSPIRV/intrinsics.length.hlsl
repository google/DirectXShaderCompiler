// Run: %dxc -T vs_6_0 -E main

// According to HLSL reference:
// The 'length' function can only operate on vector of floats.

// CHECK:      [[glsl:%\d+]] = OpExtInstImport "GLSL.std.450"

void main() {
  float result;

// CHECK:      [[c:%\d+]] = OpLoad %v3float %c
// CHECK-NEXT: [[length_c:%\d+]] = OpExtInst %float [[glsl]] Length [[c]]
// CHECK-NEXT: OpStore %result [[length_c]]
  float3 c;
  result = length(c);
}
