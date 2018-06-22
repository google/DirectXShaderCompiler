======================================
DirectXShaderCompiler on Linux & macOS
======================================

.. contents::
   :local:
   :depth: 3

Introduction
============

DirectXShaderCompiler (DXC) is based on LLVM/Clang, which is originally
cross-platform. However, to support HLSL, certain Windows specific techniques
(like COM, SAL, etc.) are introduced to solve technical issues on the Windows
platform, which also makes DXC not compilable/runnable on non-Windows platforms.

Upon `several <https://github.com/Microsoft/DirectXShaderCompiler/issues/1082>`_
requests from the community, we have started the effort to enable compilation
and running of DirectXShaderCompiler on non-Windows platforms (Linux and macOS).

Current Status
==============

Up and Running
--------------
We have currently reached the point where we can successfully build and run DXC
on Linux and macOS. We are also able to run SPIR-V CodeGen tests on these
platforms.

Known Limitations
-----------------

The following targets are currently disabled for non-Windows platforms and this
is an area where further contributions can be made:

* d3dcomp
* dxa
* dxopt
* dxl
* dxr
* dxv
* dxlib-sample

Moreover, since the HLSL CodeGen tests were originally written with Windows in
mind, they require the Windows-specific `TAEF Framework <https://docs.microsoft.com/en-us/windows-hardware/drivers/taef/>`_
to run. Therefore we are not able to compile/run these tests on non-Windows
platforms.

Known Issues
------------
Running the SPIR-V CodeGen tests results in opening a large number of file
descriptors, and if the OS limitation on the number of FDs allowed to be opened
by a process is low, it will cause test failures. We have not seen this as an
issue on Windows and Linux. On macOS we currently increase the allowed limit to
get around the problem for the time being.

Build Requirements
==================
Please make sure you have the following resources before building:

- `Git <https://git-scm.com/downloads>`_
- Ninja (can be installed from `here <https://github.com/ninja-build/ninja/releases>`_)
- Either of gcc/g++ or clang/clang++ compilers. Minimum supported version:

  - `GCC <https://gcc.gnu.org/releases.html>`_ version 5.5 or higher.
  - `clang <http://releases.llvm.org/>`_ version 3.8 or higher.


Building DirectXShaderCompiler
==============================
You can follow these steps to build DirectXShaderCompiler on Linux/macOS:

.. code:: sh

  cd <dxc-build-dir>
  cmake <dxc-src-dir> -GNinja -DCMAKE_BUILD_TYPE=Release $(cat ../utils/cmake-predefined-config-params)
  ninja

Note that ``cmake-predefined-config-params`` file contains several cmake
configurations that are needed for successful compilation. You can further
customize your build by adding configurations at the end of the cmake command
above.

For instance, you can use

``-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++``

or

``-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++``

to choose your desired C/CXX compiler.

You should now have the dxc executable located at ``<dxc-build-dir>/bin/dxc``.
And you should be able to successfully run commands as you would on Windows, e.g:

.. code:: sh

  ./bin/dxc -help
  ./bin/dxc -T <target> -E <entry-point-name> <input-hlsl-file>

Note that you cannot use slashes (``/``) for specifying command line options as
you would on Windows. You should use dashes as per usual Unix style.

Building and Running HLSL CodeGen Tests
=======================================
As described in the `Known Limitations`_ section, we can not run these tests on
non-Windows platforms due to their dependency on TAEF.

Building and Running SPIR-V CodeGen Tests
=========================================
The SPIR-V CodeGen tests were written within the googletest framework, and can
therefore be built and run on non-Windows platforms.

You can follow these steps to build and run the SPIR-V CodeGen tests:

.. code:: sh

  cd <dxc-build-dir>
  # Use SPIRV_BUILD_TESTS flag to enable building these tests.
  cmake <dxc-src-dir> -GNinja -DSPIRV_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release $(cat ../utils/cmake-predefined-config-params)
  # Build all targets. Includes 'dxc' and 'clang-spirv-tests'.
  ninja
  # Run all tests
  <dxc-build-dir>/bin/clang-spirv-tests --spirv-test-root <dxc-src-dir>/tools/clang/test/CodeGenSPIRV/


As described in the `Known Issues`_ section above, you currently need to
increase the maximum per-process open files on macOS using
``ulimit -Sn 1024`` before running the tests on that platform.

TODO: Add more information about Linux implementation details.

