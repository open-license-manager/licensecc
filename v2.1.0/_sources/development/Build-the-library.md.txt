# Build - Linux

For quick compilation instructions see the [`README.md`](https://github.com/open-license-manager/licensecc#how-to-build) file in the repository root. This page provides detailed, platform-specific instructions for Linux.

## Install prerequisites
Below the prerequisites for compiling `licensecc`.
 
### Ubuntu
Supported Ubuntu distributions are 26.04, 24.04 (Noble Numbat) and 22.04 (Jammy Jellyfish), on both x86_64 and ARM.
It should be possible to build on any recent Debian-derivate distribution.

Install prerequisites (Ubuntu 24.04 and 22.04):

```console
sudo apt-get install cmake valgrind libssl-dev zlib1g-dev libboost-test-dev libboost-filesystem-dev \
     libboost-iostreams-dev libboost-program-options-dev libboost-system-dev libboost-thread-dev \
     libboost-date-time-dev build-essential 
```

Ubuntu 26.04 additionally requires `libjitterentropy3-dev` (its OpenSSL 3.x links against the Jitter RNG entropy source):

```console
sudo apt-get install libjitterentropy3-dev
```

### Other linux
Licensecc should compile on any recent Linux distribution.

Minimum prerequisites
*   gcc => 4.9, cmake => 3.16
*   zlib, openssl => 1.1.1
*   Boost => 1.61 (If you want to compile your own boost version remember to use the flag `runtime-link=static`)

Optional prerequisites:
*   Doxygen, Sphynx for documentation

## Download and compile

### Download:
This project has a submodule (the license generator). Remember to add the option `--recursive` to clone it.

```console
git clone --recursive https://github.com/open-license-manager/licensecc.git
```

### Configure:

```console
cd licensecc/build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
```

### Compile and test:

```console
make
make install
```

```console
make test
ctest -T memcheck
```

### cmake useful flags

|Definition name           |Description|
|---------------------------|-----------|
|BOOST_ROOT <dir>          | Folder where Boost is installed. Not needed if Boost is installed with the system package manager. Boost is used only by the tests and the inspector, never by `liblicensecc` itself. |
|BUILD_SHARED_LIBS         | Build the shared (DLL/SO) variant of the library in addition to the static one. Available only when `STATIC_RUNTIME` is OFF. Default OFF. |
|CMAKE_BUILD_TYPE          | Build configuration, use `Release` for a release build (should be used as default). |
|CMAKE_INSTALL_PREFIX      | Folder where to install compiled libraries and headers. (default: /usr/local) |
|LCC_LOCATION <path>       | In case you download the license generator separately, this is the folder where it was installed or where its `lccgen-config.cmake` can be found. |
|LCC_PROJECT_NAME <str>    | Name of the software you want to protect. It is included in the license and used to name the project folders. If not specified, `DEFAULT` is used. |
|LCC_PROJECTS_BASE_DIR <dir>| Base folder where the projects are stored. Only needed if you generated a project with `lccgen` in a non default location (not in `<<CMAKE_SOURCE_DIR>>/projects/<<PROJECT_NAME>>`). |
|OPENSSL_ROOT_DIR <dir>    | Folder where OpenSSL is installed. Not needed if OpenSSL is installed as a system package. |
|STATIC_RUNTIME            | Link statically to the C/C++ runtime libraries (/MT on Windows, `-static` on Linux). Default OFF on Linux, ON on Windows. |
|USE_OPENSSL               | Enable/Disable OpenSSL support. On Linux it is mandatory and cannot be disabled. Default ON |

## Cross compile on Linux for Windows (UNTESTED)
> **Note:** The procedure below is currently untested and may be outdated
> (it was last verified on Ubuntu 18.04). It is not covered by CI —
> Windows builds are produced natively, see `.github/workflows/windows-standard.yml`.

### Prerequisites

```console
sudo apt-get install cmake valgrind binutils-mingw-w64 mingw-w64 mingw-w64-tools \ 
	mingw-w64-x86-64-dev libz-mingw-w64-dev wine-stable wine-binfmt p7zip-full
```

Download and compile boost:

```console
export CUR_PATH=$(pwd)
wget -c https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
tar xjf boost_1_71_0.tar.bz2
rm boost_1_71_0.tar.bz2
cd boost_1_71_0
sudo ln -s /usr/bin/x86_64-w64-mingw32-g++ /usr/local/bin/g++-mingw 
./bootstrap.sh
./b2 toolset=gcc-mingw target-os=windows address-model=64 --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static --prefix=./dist release install
```

Install OpenSSL:

```console
wget --no-check-certificate https://bintray.com/vszakats/generic/download_file?file_path=openssl-1.0.2h-win64-mingw.7z -O openssl.7z
7z x openssl.7z
rm openssl.7z
```
Configure and compile:
 
```
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-ubuntu-mingw64.cmake -DOPENSSL_ROOT_DIR=$CUR_PATH/openssl-OpenSSL_1_1_1d/dist -DCMAKE_FIND_DEBUG_MODE=ON -DOPENSSL_USE_STATIC_LIBS=ON -DBOOST_ROOT=$CUR_PATH/boost_1_71_0/dist  ..

```

###Build documentation

Setup the python virtual environment:

```
python3 -m venv .venv

. .venv/bin/activate
pip install wheel
pip install -r requirements.txt

```

Build the docs:

```
. .venv/bin/activate
cd build
cmake ..
make documentation
```