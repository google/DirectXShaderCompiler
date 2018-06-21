=======================================
DirectXShaderCompiler on Linux & Mac OS
=======================================

.. contents::
   :local:
   :depth: 3

Introduction
============

Due to several `requests <https://github.com/Microsoft/DirectXShaderCompiler/issues/1236>`_
from the community, we have started the effort to enable compilation and running
of DirectXShaderCompiler on non-Windows platforms (Linux and Mac OS).


Building DirectXShaderCompiler
==============================
You can follow these steps to build DirectXShaderCompiler on Linux/MacOS:

(We are assuming you have cloned the repository to `~/DirectXShaderCompiler`)


.. code:: sh

  cd ~/DirectXShaderCompiler
  mkdir build && cd build
  cmake .. -GNinja                                 \
    $(cat ../utils/cmake-predefined-config-params) \
    -DSPIRV_BUILD_TESTS=ON                         \
    -DCMAKE_BUILD_TYPE=${DXC_BUILD_TYPE}           \
    -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX}

  ninja dxc


``DXC_BUILD_TYPE`` is the cmake build type (``Debug`` or ``Release`` or
``RelWithDebInfo``).
You can use ``clang`` and ``clang++``, or ``gcc`` and ``g++`` as your C/CXX
compiler.

You should now have the dxc executable located at `~/DirectXShaderCompiler/build/bin/dxc`.

And you should be able to successfully run commands as you would on Windows, e.g:

.. code:: sh

  ./bin/dxc -help
  ./bin/dxc -T <target> -E <entry> <InputFile.hlsl>
  ./bin/dxc -T <target> -E <entry> <InputFile.hlsl> -ast-dump


Building and Running HLSL CodeGen Tests
=======================================
Since the unit tests and HLSL CodeGen tests were originally written with Windows
in mind, they require the Windows-specific `TAEF Framework <https://docs.microsoft.com/en-us/windows-hardware/drivers/taef/>`_ to run. Therefore we are not able to run these tests on
non-Windows platforms.

Building and Running SPIR-V CodeGen Tests
=========================================
The SPIR-V CodeGen tests were written within the googletest framework, and can
therefore be built and run on non-Windows platforms as well.

You can follow these steps to build and run the SPIR-V CodeGen tests:

(We are assuming that you have already built dxc using the above instructions)

.. code:: sh

  # Build clang-spirv-tests
  ninja clang-spirv-tests

  # Run all tests
  ./bin/clang-spirv-tests --spirv-test-root ../tools/clang/test/CodeGenSPIRV/


TODO: Add more information about Linux implementation details.

