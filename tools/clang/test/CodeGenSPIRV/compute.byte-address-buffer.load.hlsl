// Run: %dxc -T cs_6_0 -E main

// CHECK: %_ptr_Uniform_uint = OpTypePointer Uniform %uint
// CHECK: %myBuffer = OpVariable %_ptr_Uniform_type_ByteAddressBuffer Uniform
ByteAddressBuffer myBuffer;

[numthreads(1, 1, 1)]
void main() {
  uint addr = 0;

// CHECK: [[addr1:%\d+]] = OpLoad %uint %addr
// CHECK-NEXT: [[word_addr:%\d+]] = OpShiftRightLogical %uint [[addr1]] %uint_2
// CHECK-NEXT: [[load_ptr:%\d+]] = OpAccessChain %_ptr_Uniform_uint %myBuffer %uint_0 [[word_addr]]
// CHECK-NEXT: {{%\d+}} = OpLoad %uint [[load_ptr]]
  uint word = myBuffer.Load(addr);


// CHECK: [[addr:%\d+]] = OpLoad %uint %addr
// CHECK-NEXT: [[load4_word0Addr:%\d+]] = OpShiftRightLogical %uint [[addr]] %uint_2
// CHECK-NEXT: [[load_ptr3:%\d+]] = OpAccessChain %_ptr_Uniform_uint %myBuffer %uint_0 [[load4_word0Addr]]
// CHECK-NEXT: [[load4_word0:%\d+]] = OpLoad %uint [[load_ptr3]]
// CHECK-NEXT: [[load4_word1Addr:%\d+]] = OpIAdd %uint [[load4_word0Addr]] %uint_1
// CHECK-NEXT: [[load_ptr4:%\d+]] = OpAccessChain %_ptr_Uniform_uint %myBuffer %uint_0 [[load4_word1Addr]]
// CHECK-NEXT: [[load4_word1:%\d+]] = OpLoad %uint [[load_ptr4]]
// CHECK-NEXT: [[load4_word2Addr:%\d+]] = OpIAdd %uint [[load4_word0Addr]] %uint_2
// CHECK-NEXT: [[load_ptr5:%\d+]] = OpAccessChain %_ptr_Uniform_uint %myBuffer %uint_0 [[load4_word2Addr]]
// CHECK-NEXT: [[load4_word2:%\d+]] = OpLoad %uint [[load_ptr5]]
// CHECK-NEXT: [[load4_word3Addr:%\d+]] = OpIAdd %uint [[load4_word0Addr]] %uint_3
// CHECK-NEXT: [[load_ptr6:%\d+]] = OpAccessChain %_ptr_Uniform_uint %myBuffer %uint_0 [[load4_word3Addr]]
// CHECK-NEXT: [[load4_word3:%\d+]] = OpLoad %uint [[load_ptr6]]
// CHECK-NEXT: {{%\d+}} = OpCompositeConstruct %v4uint [[load4_word0]] [[load4_word1]] [[load4_word2]] [[load4_word3]]
  uint4 word4 = myBuffer.Load4(addr);
}
