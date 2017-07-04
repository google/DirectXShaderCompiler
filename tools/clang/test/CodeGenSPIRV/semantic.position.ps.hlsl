// Run: %dxc -T ps_6_0 -E main

// CHECK:                 OpEntryPoint Fragment %main "main" {{%\d+}} %gl_FragCoord

// CHECK:                 OpDecorate %gl_FragCoord BuiltIn FragCoord

// CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input

float4 main(float4 input: SV_Position) : SV_Target {
    return input;
}
