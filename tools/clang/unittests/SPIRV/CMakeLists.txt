set(LLVM_LINK_COMPONENTS
  Support
  dxcsupport
  hlsl
  )

add_clang_unittest(clang-spirv-tests
  InstBuilderTest.cpp
  DecorationTest.cpp
  SPIRVContextTest.cpp
  ModuleBuilderTest.cpp
  TypeTest.cpp
  UtilsTest.cpp
  )

target_link_libraries(clang-spirv-tests
  clangCodeGen
  clangFrontend
  clangSPIRV
  )
