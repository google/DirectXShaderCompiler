set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangSPIRV
  EmitSPIRVAction.cpp
  InstBuilderAuto.cpp
  InstBuilderManual.cpp
  SPIRVBuilder.cpp

  LINK_LIBS
  clangAST
  clangBasic
  clangFrontend
  clangLex
  )
