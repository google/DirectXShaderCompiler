// Run: %dxc -T vs_6_0 -E main

// According to HLSL reference:
// The components of the vectors may be either float or int.

void main() {
    // Test int1
    int1 a, b;
    int c;

    // Test int2
    int2 d, e;
    int f;

    // Test int3
    int3 g, h;
    int i;

    // Test int4
    int4 j, k;
    int l;

    // Test float1
    float1 m, n;
    float o;

    // Test float2
    float2 p, q;
    float r;

    // Test float3
    float3 s, t;
    float u;

    // Test float4
    float4 v, w;
    float x;

    // CHECK:      [[a:%\d+]] = OpLoad %int %a
    // CHECK-NEXT: [[b:%\d+]] = OpLoad %int %b
    // CHECK-NEXT: [[intdot1:%\d+]] = OpIMul %int [[a]] [[b]]
    // CHECK-NEXT: OpStore %c [[intdot1]]
    c = dot(a, b);

    // CHECK:      [[d:%\d+]] = OpLoad %v2int %d
    // CHECK-NEXT: [[e:%\d+]] = OpLoad %v2int %e
    // CHECK-NEXT: [[d0:%\d+]] = OpCompositeExtract %int [[d]] 0
    // CHECK-NEXT: [[e0:%\d+]] = OpCompositeExtract %int [[e]] 0
    // CHECK-NEXT: [[mul_de0:%\d+]] = OpIMul %int [[d0]] [[e0]]
    // CHECK-NEXT: [[d1:%\d+]] = OpCompositeExtract %int [[d]] 1
    // CHECK-NEXT: [[e1:%\d+]] = OpCompositeExtract %int [[e]] 1
    // CHECK-NEXT: [[mul_de1:%\d+]] = OpIMul %int [[d1]] [[e1]]
    // CHECK-NEXT: [[intdot2:%\d+]] = OpIAdd %int [[mul_de0]] [[mul_de1]]
    // CHECK-NEXT: OpStore %f [[intdot2]]
    f = dot(d, e);

    // CHECK:      [[g:%\d+]] = OpLoad %v3int %g
    // CHECK-NEXT: [[h:%\d+]] = OpLoad %v3int %h
    // CHECK-NEXT: [[g0:%\d+]] = OpCompositeExtract %int [[g]] 0
    // CHECK-NEXT: [[h0:%\d+]] = OpCompositeExtract %int [[h]] 0
    // CHECK-NEXT: [[mul_gh0:%\d+]] = OpIMul %int [[g0]] [[h0]]
    // CHECK-NEXT: [[g1:%\d+]] = OpCompositeExtract %int [[g]] 1
    // CHECK-NEXT: [[h1:%\d+]] = OpCompositeExtract %int [[h]] 1
    // CHECK-NEXT: [[mul_gh1:%\d+]] = OpIMul %int [[g1]] [[h1]]
    // CHECK-NEXT: [[g2:%\d+]] = OpCompositeExtract %int [[g]] 2
    // CHECK-NEXT: [[h2:%\d+]] = OpCompositeExtract %int [[h]] 2
    // CHECK-NEXT: [[mul_gh2:%\d+]] = OpIMul %int [[g2]] [[h2]]
    // CHECK-NEXT: [[intdot3_add0:%\d+]] = OpIAdd %int [[mul_gh0]] [[mul_gh1]]
    // CHECK-NEXT: [[intdot3:%\d+]] = OpIAdd %int [[intdot3_add0]] [[mul_gh2]]
    // CHECK-NEXT: OpStore %i [[intdot3]]
    i = dot(g, h);

    // CHECK:      [[j:%\d+]] = OpLoad %v4int %j
    // CHECK-NEXT: [[k:%\d+]] = OpLoad %v4int %k
    // CHECK-NEXT: [[j0:%\d+]] = OpCompositeExtract %int [[j]] 0
    // CHECK-NEXT: [[k0:%\d+]] = OpCompositeExtract %int [[k]] 0
    // CHECK-NEXT: [[mul_jk0:%\d+]] = OpIMul %int [[j0]] [[k0]]
    // CHECK-NEXT: [[j1:%\d+]] = OpCompositeExtract %int [[j]] 1
    // CHECK-NEXT: [[k1:%\d+]] = OpCompositeExtract %int [[k]] 1
    // CHECK-NEXT: [[mul_jk1:%\d+]] = OpIMul %int [[j1]] [[k1]]
    // CHECK-NEXT: [[j2:%\d+]] = OpCompositeExtract %int [[j]] 2
    // CHECK-NEXT: [[k2:%\d+]] = OpCompositeExtract %int [[k]] 2
    // CHECK-NEXT: [[mul_jk2:%\d+]] = OpIMul %int [[j2]] [[k2]]
    // CHECK-NEXT: [[j3:%\d+]] = OpCompositeExtract %int [[j]] 3
    // CHECK-NEXT: [[k3:%\d+]] = OpCompositeExtract %int [[k]] 3
    // CHECK-NEXT: [[mul_jk3:%\d+]] = OpIMul %int [[j3]] [[k3]]
    // CHECK-NEXT: [[intdot4_add0:%\d+]] = OpIAdd %int [[mul_jk0]] [[mul_jk1]]
    // CHECK-NEXT: [[intdot4_add1:%\d+]] = OpIAdd %int [[intdot4_add0]] [[mul_jk2]]
    // CHECK-NEXT: [[intdot4:%\d+]] = OpIAdd %int [[intdot4_add1]] [[mul_jk3]]
    // CHECK-NEXT: OpStore %l [[intdot4]]
    l = dot(j, k);

    // CHECK:      [[m:%\d+]] = OpLoad %float %m
    // CHECK-NEXT: [[n:%\d+]] = OpLoad %float %n
    // CHECK-NEXT: [[floatdot1:%\d+]] = OpFMul %float [[m]] [[n]]
    // CHECK-NEXT: OpStore %o [[floatdot1]]
    o = dot(m, n);

    // CHECK:      [[p:%\d+]] = OpLoad %v2float %p
    // CHECK-NEXT: [[q:%\d+]] = OpLoad %v2float %q
    // CHECK-NEXT: [[floatdot2:%\d+]] = OpDot %float [[p]] [[q]]
    // CHECK-NEXT: OpStore %r [[floatdot2]]
    r = dot(p, q);

    // CHECK:      [[s:%\d+]] = OpLoad %v3float %s
    // CHECK-NEXT: [[t:%\d+]] = OpLoad %v3float %t
    // CHECK-NEXT: [[floatdot3:%\d+]] = OpDot %float [[s]] [[t]]
    // CHECK-NEXT: OpStore %u [[floatdot3]]
    u = dot(s, t);

    // CHECK:      [[v:%\d+]] = OpLoad %v4float %v
    // CHECK-NEXT: [[w:%\d+]] = OpLoad %v4float %w
    // CHECK-NEXT: [[floatdot4:%\d+]] = OpDot %float [[v]] [[w]]
    // CHECK-NEXT: OpStore %x [[floatdot4]]
    x = dot(v, w);
}
