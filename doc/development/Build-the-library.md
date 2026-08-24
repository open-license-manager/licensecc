# Build - Linux

## Install prerequisites
Below the prerequisites for compiling `licensecc`. For developing it we use Eclipse. 
Recent CDT works smoothly with CMake. Remember to install the Ninja package as build system and Cmake Gui for a good eclipse integration.
 
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

For development with eclipse:

```console
sudo apt-get install cmake-gui ninja-build
```

### Other linux
Licensecc should compile on any recent Linux distribution.

Minimum prerequisites
*   gcc => 4.9, cmake => 3.16
*   zlib, openssl => 1.0.2
*   Boost => 1.57 (If you want to compile your own boost version remember to use the flag `runtime-link=static`)

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
|--------------------------|-----------|
|LCC_PROJECT_NAME=<str>  | This correspond to the name of the project you're generating licenses for. The flag is optional, if you don't specify it the build system will create a project named `DEFAULT` for you |
|LCC_LOCATION=<path>     | In case you download the license generator separately this is the folder where it was installed or where his lcc-config.cmake can be found |
|CMAKE_BUILD_TYPE=Release| generate a release version of the library (should be used as default)|
|CMAKE_INSTALL_PREFIX    | folder where to install compiled libraries and headers. (default: /usr/local)               |
|BOOST_ROOT              | Folder where boost was installed (optional: if you installed boost using system package manager this should not be necessary) |
|OPENSSL_ROOT            | Folder where OpenSSL was installed (optional: if you installed openssl as system package this should not be necessary) |

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