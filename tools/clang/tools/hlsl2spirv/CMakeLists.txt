# Builds hlsl2spirv.exe

set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  dxcsupport
  hlsl
  option
  support
  )

add_clang_executable(hlsl2spirv
  hlsl2spirv.cpp
  )

target_link_libraries(hlsl2spirv
  dxcompiler
  SPIRV-Tools
  )

set_target_properties(hlsl2spirv PROPERTIES VERSION ${CLANG_EXECUTABLE_VERSION})

add_dependencies(hlsl2spirv dxcompiler)

if(UNIX)
  set(CLANGXX_LINK_OR_COPY create_symlink)
# Create a relative symlink
  set(hlsl2spirv_binary "hlsl2spirv${CMAKE_EXECUTABLE_SUFFIX}")
else()
  set(CLANGXX_LINK_OR_COPY copy)
  set(hlsl2spirv_binary "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/hlsl2spirv${CMAKE_EXECUTABLE_SUFFIX}")
endif()

install(TARGETS hlsl2spirv
  RUNTIME DESTINATION bin)
