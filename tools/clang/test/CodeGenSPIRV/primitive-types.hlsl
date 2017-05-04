// Run: %dxc -T ps_6_0 -E main
void main() {
  bool a;
  int b;
  uint c;
  dword d;
  float e;
  bool1 a1;
  int1 b1;
  uint1 c1;
  dword1 d1;
  float1 e1;
}

// TODO
// - 16bit & 64bit integers/floats (requires additional capabilities)

// CHECK-WHOLE-SPIR-V:
// OpCapability Shader
// OpMemoryModel Logical GLSL450
// OpEntryPoint Fragment %main "main"
// OpExecutionMode %main OriginUpperLeft
// OpName %main "main"
// OpName %bb_entry "bb.entry"
// OpName %a "a"
// OpName %b "b"
// OpName %c "c"
// OpName %d "d"
// OpName %e "e"
// OpName %a1 "a1"
// OpName %b1 "b1"
// OpName %c1 "c1"
// OpName %d1 "d1"
// OpName %e1 "e1"
// %int = OpTypeInt 32 1
// %uint = OpTypeInt 32 0
// %void = OpTypeVoid
// %2 = OpTypeFunction %void
// %bool = OpTypeBool
// %_ptr_Function_bool = OpTypePointer Function %bool
// %_ptr_Function_int = OpTypePointer Function %int
// %_ptr_Function_uint = OpTypePointer Function %uint
// %float = OpTypeFloat 32
// %_ptr_Function_float = OpTypePointer Function %float
// %main = OpFunction %void None %2
// %bb_entry = OpLabel
// %a = OpVariable %_ptr_Function_bool Function
// %b = OpVariable %_ptr_Function_int Function
// %c = OpVariable %_ptr_Function_uint Function
// %d = OpVariable %_ptr_Function_uint Function
// %e = OpVariable %_ptr_Function_float Function
// %a1 = OpVariable %_ptr_Function_bool Function
// %b1 = OpVariable %_ptr_Function_int Function
// %c1 = OpVariable %_ptr_Function_uint Function
// %d1 = OpVariable %_ptr_Function_uint Function
// %e1 = OpVariable %_ptr_Function_float Function
// OpReturn
// OpFunctionEnd