# Windows

This page describes how to build the library. It's a good starting point if you just downloaded it and you want to start explore.

Under windows both MSVC compiler and MinGW are supported

## MSVC (2015~2019) - prerequisites
Pre-compiled [versions of boost](https://sourceforge.net/projects/boost/files/boost-binaries/) for windows are available at SourceForge. 

For MSVC 2019: recommended cmake => 3.15.2 (the version 3.14 embedded in visual studio is not working with boost 1.71), [boost 1.71 msvc-14.2](https://dl.bintray.com/boostorg/release/1.71.0/binaries/boost_1_71_0-msvc-14.2-64.exe)

### Download:
This project has a submodule (the license generator). Remember to add the option `--recursive` to clone it.

```console
git clone --recursive https://github.com/open-license-manager/open-license-manager.git
```

~~Initialize command line tools:~~

```
~~ "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 ~~
```

Configure the library:

```
cmake .. -G "Visual Studio 16 2019" -A x64 -DBOOST_ROOT="C:\local\boost"  //(or where boost was installed)
```

### Supported cmake definitions/options
Below a list of some useful cmake configurations 

|Definition name|Description|
|---------------|-----------|
|-DSTATIC_RUNTIME=1        |link statically to the standard and runtime support libraries (compiler flag /MT)|
|-DCMAKE_BUILD_TYPE=Release|link to the release version of the boost libraries|
|-DCMAKE_INSTALL_PREFIX=C:\.. | folder where to install libraries and headers |
|-DBOOST_ROOT=C:\.. | folder where boost is installed. If cmake is reporting boost not found consider updating cmake. |

Compile and test (command line)

```
cmake --build . --target install --config Release

ctest -C Release
```