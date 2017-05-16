// Run: %dxc -T ps_6_0 -E main

void main() {
// CHECK-LABEL: %bb_entry = OpLabel

    bool  r1;
    bool2 r2;
    bool3 r3;
    bool4 r4;

    int1  a, b;
    int2   i, j;
    uint3  m, n;
    float4 o, p;

// CHECK:      [[a:%\d+]] = OpLoad %int %a
// CHECK-NEXT: [[b:%\d+]] = OpLoad %int %b
// CHECK-NEXT: [[lt1:%\d+]] = OpSLessThan %bool [[a]] [[b]]
// CHECK-NEXT: OpStore %r1 [[lt1]]
    r1 = a < b;
// CHECK-NEXT: [[i:%\d+]] = OpLoad %v2int %i
// CHECK-NEXT: [[j:%\d+]] = OpLoad %v2int %j
// CHECK-NEXT: [[lt2:%\d+]] = OpSLessThan %v2bool [[i]] [[j]]
// CHECK-NEXT: OpStore %r2 [[lt2]]
    r2 = i < j;
// CHECK-NEXT: [[m:%\d+]] = OpLoad %v3uint %m
// CHECK-NEXT: [[n:%\d+]] = OpLoad %v3uint %n
// CHECK-NEXT: [[lt3:%\d+]] = OpULessThan %v3bool [[m]] [[n]]
// CHECK-NEXT: OpStore %r3 [[lt3]]
    r3 = m < n;
// CHECK-NEXT: [[o:%\d+]] = OpLoad %v4float %o
// CHECK-NEXT: [[p:%\d+]] = OpLoad %v4float %p
// CHECK-NEXT: [[lt4:%\d+]] = OpFOrdLessThan %v4bool [[o]] [[p]]
// CHECK-NEXT: OpStore %r4 [[lt4]]
    r4 = o < p;
}
