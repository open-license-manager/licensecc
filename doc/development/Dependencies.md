# Dependencies

This page describes the dependencies of `licensecc` and the supported build environments.

Dependencies varies with the environment, if you're building the library for the first time we suggest you download or set up one of the supported environments to be sure not to incur in dependency/compiler errors (you can use virtual machines , docker or lxc/lxd technologies).

The library is composed by two modules: a license generator `lcc` executable, that also works as a project configuration tool, and the C++ library itself `licensecc` (the part you have to integrate in your application).
 
## Library `licensecc` dependencies
These are the dependencies of the library you have to link to your code. We try to keep them at a minimum. N.B. boost is always optional and it's never linked to your application.

| Operating System      | Openssl    | Other                       | Boost<sup>2</sup>   |
|-----------------------|:----------:|:---------------------------:|:-------------------:| 
| Ubuntu 26.04     | :heavy_check_mark: => v1.1 | libjitterentropy3-dev | optional(test) => 1.64 |
| Ubuntu 24.04     | :heavy_check_mark: => v1.1 |                       | optional(test) => 1.64 |
| Ubuntu 22.04     | :heavy_check_mark: => v1.1 |                       | optional(test) => 1.64 |
| Windows MSVC 2022| optional<sup>1</sup>       |                       | optional(test) => 1.64 |
| Windows MinGW<sup>4</sup>| optional<sup>1</sup>       |               | optional(test) => 1.64 |

You may have to install ZLib if your openssl version was compiled with it, in Debian it comes via dependency mechanism.

## License generator executable `lccgen` dependencies

| Operating System               | Openssl      | Other                      | Boost<sup>3</sup>         |
|--------------------------------|:------------:|:--------------------------:|:-----------------------:| 
| Ubuntu 26.04        |:heavy_check_mark:  => v1.1 | libjitterentropy3-dev   |:heavy_check_mark: => 1.64 |
| Ubuntu 24.04        |:heavy_check_mark:  => v1.1 |                         |:heavy_check_mark: => 1.64 |
| Ubuntu 22.04        |:heavy_check_mark:  => v1.1 |                         |:heavy_check_mark: => 1.64 |
| Windows MSVC 2022   | optional<sup>1</sup>       |                         |:heavy_check_mark: => 1.64 |
| Windows MinGW<sup>4</sup>| optional<sup>1</sup>|                           |:heavy_check_mark: => 1.64 |

You may have to install ZLib if your openssl version was compiled with it.

Notes:
<sup>1</sup> There is no added feature in compiling `licensecc` with OpenSSL under windows, do so if it's already part of your project otherwise go without. 

<sup>2</sup> Boost components required to run tests `unit_test_framework`,`system` (boost < 1.90),`filesystem`. But then if you don't want to install boost since `licensecc` uses `lccgen` in the build process you need to download and install `lccgen` separately.

<sup>3</sup> Boost is mandadory dependency of `lccgen` executable. Components: `unit_test_framework`,`system` (boost < 1.90),`filesystem`

<sup>4</sup> MinGW is not tested (=not supported) in version `licensecc` version 2.5.0. It was working in 2.0.0.