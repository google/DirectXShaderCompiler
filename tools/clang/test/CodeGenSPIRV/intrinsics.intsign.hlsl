// Run: %dxc -T vs_6_0 -E main

// According to HLSL reference:
// The 'sign' function can operate on int, int vectors, and int matrices.

// CHECK:      [[glsl:%\d+]] = OpExtInstImport "GLSL.std.450"

void main() {
  int result;
  int3 result3;

// CHECK:      [[a:%\d+]] = OpLoad %int %a
// CHECK-NEXT: [[sign_a:%\d+]] = OpExtInst %int [[glsl]] SSign [[a]]
// CHECK-NEXT: OpStore %result [[sign_a]]
  int a;
  result = sign(a);

// CHECK-NEXT: [[b:%\d+]] = OpLoad %int %b
// CHECK-NEXT: [[sign_b:%\d+]] = OpExtInst %int [[glsl]] SSign [[b]]
// CHECK-NEXT: OpStore %result [[sign_b]]
  int1 b;
  result = sign(b);

// CHECK-NEXT: [[c:%\d+]] = OpLoad %v3int %c
// CHECK-NEXT: [[sign_c:%\d+]] = OpExtInst %v3int [[glsl]] SSign [[c]]
// CHECK-NEXT: OpStore %result3 [[sign_c]]
  int3 c;
  result3 = sign(c);
  
// TODO: Integer matrices are not supported yet. Therefore we cannot run the following test yet.
// XXXXX-NEXT: [[h:%\d+]] = OpLoad %mat3v4int %h
// XXXXX-NEXT: [[h_row0:%\d+]] = OpCompositeExtract %v4int [[h]] 0
// XXXXX-NEXT: [[SSign_h_row0:%\d+]] = OpExtInst %v4int [[glsl]] SSign [[h_row0]]
// XXXXX-NEXT: [[h_row1:%\d+]] = OpCompositeExtract %v4int [[h]] 1
// XXXXX-NEXT: [[SSign_h_row1:%\d+]] = OpExtInst %v4int [[glsl]] SSign [[h_row1]]
// XXXXX-NEXT: [[h_row2:%\d+]] = OpCompositeExtract %v4int [[h]] 2
// XXXXX-NEXT: [[SSign_h_row2:%\d+]] = OpExtInst %v4int [[glsl]] SSign [[h_row2]]
// XXXXX-NEXT: [[SSign_matrix:%\d+]] = OpCompositeConstruct %mat3v4int [[SSign_h_row0]] [[SSign_h_row1]] [[SSign_h_row2]]
// XXXXX-NEXT: OpStore %result3x4 [[SSign_matrix]]
//  int3x4 h;
//  int3x4 result3x4 = sign(h);
}
