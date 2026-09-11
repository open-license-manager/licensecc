#######################################
Build - Windows
#######################################

This page describes how to build the library under windows with licensecc 2.5.0.

For quick compilation instructions see the `README.md <https://github.com/open-license-manager/licensecc#how-to-build>`_ file in the repository root. This page provides detailed, platform-specific instructions for Windows.

MSVC
*****************

Supported Visual Studio versions are:

* Visual Studio 2026 (used in development).
* Visual Studio 2022 (used in the automated builds). The lowest
  supported version is the latest minor release/servicing update of Visual Studio 2022
  (v17.x).
* Visual Studio 2019 and 2017 are not tested anymore (though they should still work).

Automated tests run on Windows Server 2022, Windows Server 2026 and Windows 11 (arm64).

Libraries supported/tested in 2.5.0 (Windows x64):

* CMake: >= 3.16.
* Boost: tested with 1.64.0, 1.78.0 and 1.90.0. Boost is only needed to run the tests
  and to build ``lccgen`` and ``lcc-inspector``; it's never linked into ``liblicensecc``.
  The pre-compiled binaries should match the compiler version.
* OpenSSL: optional. 

MSVC install prerequisites
============================= 
Git is of course a prerequisite, if you don't have it you can download it from `git-scm.com <https://git-scm.com/download/win>`_. 

Pre-compiled versions of boost for windows can be downloaded from the
`userdocs/boost <https://github.com/userdocs/boost/releases>`_ releases (the same source
used by the project CI). Choose the installer that matches the desired architecture and
compiler, eg. for Visual Studio 2022 64 bit download ``boost_1_90_0-msvc-14.3-64.exe``.

Alternatively pre-compiled boost binaries are available at
`SourceForge <https://sourceforge.net/projects/boost/files/boost-binaries/>`_.

Checkout the code
==================
Check out the code using git:

.. NOTE::
 
  This project has a submodule (the license generator). Remember to add the option `--recursive` to clone it.

.. code-block:: console

  git clone --recursive https://github.com/open-license-manager/licensecc.git

Compile and build (command line)
====================================

Open a command prompt in the folder where you checked out the library.

Configure the library (windows x64):
 
.. code-block:: console
  
  cd build
  cmake .. -G "Visual Studio 17 2022" -A x64 -DBOOST_ROOT="C:\local\boost"  //(or where boost was installed)

Configure the library (windows x86):

For some configuration reason we're unable to build using x86 using visual studio generators. We recommend to use Ninja
build system. 

.. code-block:: console

  cd build
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

  cmake .. -G "Ninja" [-DBOOST_ROOT="C:\local\boost"  //(or where boost was installed)]

Supported cmake definitions/options
=======================================
Below a list of some useful cmake configurations:

============================== ====================================================================================================
Definition name                Description
============================== ====================================================================================================
BOOST_ROOT=C:\..               Folder where Boost is installed. Not needed if Boost is installed with the system package manager. Boost is used only by the tests and the inspector, never by ``liblicensecc`` itself. If cmake is reporting Boost not found consider updating cmake.
BUILD_SHARED_LIBS=OFF          Additionally build the shared (DLL) version of the library. Requires -DSTATIC_RUNTIME=OFF.
CMAKE_BUILD_TYPE=Release       Build configuration, use Release for a release build (should be used as default).
CMAKE_INSTALL_PREFIX=C:\XX     Folder where to install libraries and headers.
LCC_PROJECT_NAME=<name>        Name of the software you want to issue a license for. Defaults to "DEFAULT".
OPENSSL_ROOT_DIR=C:\..         (Optional) Folder where OpenSSL is installed (eg. C:\Program Files\OpenSSL-Win64). Only used when OpenSSL support is enabled.
STATIC_RUNTIME=ON              Link statically to the standard and runtime support libraries (compiler flag /MT). Default ON.
USE_OPENSSL=OFF                Enable/Disable OpenSSL support (search for and link against OpenSSL). Default OFF on Windows: the Windows crypto APIs are used instead, no additional feature from OpenSSL. If enabled and OpenSSL is not found the build fails.
OPENSSL_USE_STATIC_LIBS=ON     Link against the static or dynamic version of openssl libraries (if OpenSSL selected) 
============================== ====================================================================================================

Compile and test 

.. code-block:: console
  
  cmake --build . --target install --config Release
  ctest -C Release


Compile and build (Visual studio 2026)
==========================================

Visual Studio 2022 integrates with CMake (the process requires a couple of restarts and it's all but "fluid").

After opening the project "as a CMake project" a CMakeSettings.json should appear in the base folder. Edit the file as 
below (the file is for an x86 architecture). In a special way:

* remove the "-v" switch from "buildCommandArgs" 
* add the variable BOOST_ROOT pointing to where your boost installation is.

Restart, delete and rebuild the cache a couple of time, until Visual Studio understands the new options.

.. literalinclude:: CMakeSettings.json
   :language: json


Windows ARM64
*****************
Windows on ARM (arm64) is supported and tested in CI on a Windows 11 arm64 runner.
Boost is provided by `vcpkg <https://vcpkg.io/>`_ (packages ``boost-program-options``,
``boost-test``, ``boost-filesystem``, ``boost-date-time`` with the
``arm64-windows-static-release`` triplet) and OpenSSL is disabled (the Windows
cryptography APIs are used):

.. code-block:: console

  cmake -S . -B build -DSTATIC_RUNTIME=ON -DUSE_OPENSSL=OFF -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_TOOLCHAIN_FILE="<vcpkg folder>\scripts\buildsystems\vcpkg.cmake" ^
        -DVCPKG_TARGET_TRIPLET=arm64-windows-static-release
  cmake --build build --target install --config Release
  ctest -C Release --test-dir build
   

MINGW 
*****************
Mingw is not tested (=unsupported) in 2.5.0 but it was by version 2.0 and it may be in future. 
This section is a placeholder from the old release (BTW if it works please comment in the forum)

.. TODO::
   
   Describe how to install and configure mingw

Prerequistites:

* Powershell
* 7zip
* git
* cmake

Install and compile boost:

.. code-block:: console

    wget https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.7z
    7z x boost_1_64_0.7z -oC:/local
    cd "C:\local\boost_1_64_0"
    bootstrap.bat gcc 
    b2.exe -d0 --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static toolset=gcc --prefix=C:\local\boost_1_68_0\boost-mingw install
    cd C:/local/boost_1_64_0/boost-mingw/lib
    dir 

Verify boost is really compiled. Go to the folder where you want to download `licensecc` 

.. code-block:: console

   git clone --recursive https://github.com/open-license-manager/licensecc.git
   cd licensecc/build && cmake -G "MinGW Makefiles" -DBOOST_ROOT="C:/local/boost_1_68_0/boost-mingw" -DBoost_ARCHITECTURE="-x64" -DCMAKE_CXX_COMPILER_ARCHITECTURE_ID="x64" -DCMAKE_SH="CMAKE_SH-NOTFOUND" ..
   cmake --build . --target install --config Release

And then you can test it:
   
.. code-block:: console

   ctest -C Release



