// Run: %dxc -T cs_6_0 -E main

// CHECK: %_ptr_Uniform_type_RWByteAddressBuffer = OpTypePointer Uniform %type_RWByteAddressBuffer
// CHECK: %outBuffer = OpVariable %_ptr_Uniform_type_RWByteAddressBuffer Uniform
RWByteAddressBuffer outBuffer;

[numthreads(1, 1, 1)]
void main() {
  uint addr = 0;
  uint4 words = uint4(1, 2, 3, 4);

// CHECK: [[words:%\d+]] = OpLoad %v4uint %words
// CHECK-NEXT: [[byteAddr:%\d+]] = OpLoad %uint %addr
// CHECK-NEXT: [[baseAddr:%\d+]] = OpShiftRightLogical %uint [[byteAddr]] %uint_2

// CHECK-NEXT: [[word0:%\d+]] = OpCompositeExtract %uint [[words]] 0
// CHECK-NEXT: [[outBufPtr0:%\d+]] = OpAccessChain %_ptr_Uniform_uint %outBuffer %uint_0 [[baseAddr]]
// CHECK-NEXT: OpStore [[outBufPtr0]] [[word0]]

// CHECK-NEXT: [[word1:%\d+]] = OpCompositeExtract %uint [[words]] 1
// CHECK-NEXT: [[baseAddr_plus1:%\d+]] = OpIAdd %uint [[baseAddr]] %uint_1
// CHECK-NEXT: [[outBufPtr1:%\d+]] = OpAccessChain %_ptr_Uniform_uint %outBuffer %uint_0 [[baseAddr_plus1]]
// CHECK-NEXT: OpStore [[outBufPtr1]] [[word1]]

// CHECK-NEXT: [[word2:%\d+]] = OpCompositeExtract %uint [[words]] 2
// CHECK-NEXT: [[baseAddr_plus2:%\d+]] = OpIAdd %uint [[baseAddr]] %uint_2
// CHECK-NEXT: [[outBufPtr2:%\d+]] = OpAccessChain %_ptr_Uniform_uint %outBuffer %uint_0 [[baseAddr_plus2]]
// CHECK-NEXT: OpStore [[outBufPtr2]] [[word2]]

// CHECK-NEXT: [[word3:%\d+]] = OpCompositeExtract %uint [[words]] 3
// CHECK-NEXT: [[baseAddr_plus3:%\d+]] = OpIAdd %uint [[baseAddr]] %uint_3
// CHECK-NEXT: [[outBufPtr3:%\d+]] = OpAccessChain %_ptr_Uniform_uint %outBuffer %uint_0 [[baseAddr_plus3]]
// CHECK-NEXT: OpStore [[outBufPtr3]] [[word3]]
  outBuffer.Store4(addr, words);
}
