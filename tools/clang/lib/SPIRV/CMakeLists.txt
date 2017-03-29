set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangSPIRV
  Decoration.cpp
  EmitSPIRVAction.cpp
  InstBuilderAuto.cpp
  InstBuilderManual.cpp
  Structure.cpp
  ModuleBuilder.cpp
  SPIRVContext.cpp
  Type.cpp
  Utils.cpp

  LINK_LIBS
  clangAST
  clangBasic
  clangFrontend
  clangLex
  )
