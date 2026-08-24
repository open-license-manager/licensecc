# Licensecc

*Copy protection, licensing library and license generator for Windows and Linux.*

[![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Linux build](https://github.com/open-license-manager/licensecc/actions/workflows/linux-standard.yml/badge.svg)](https://github.com/open-license-manager/licensecc/actions/workflows/linux-standard.yml)
[![Windows build](https://github.com/open-license-manager/licensecc/actions/workflows/windows-standard.yml/badge.svg)](https://github.com/open-license-manager/licensecc/actions/workflows/windows-standard.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81a1f6bc15014618934fc5fab4d3c206)](https://www.codacy.com/gh/open-license-manager/licensecc/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=open-license-manager/licensecc&amp;utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/open-license-manager/licensecc/branch/develop/graph/badge.svg?token=vdrBBzX6Rl)](https://codecov.io/gh/open-license-manager/licensecc)
[![Github Issues](https://img.shields.io/github/issues/open-license-manager/licensecc)](http://github.com/open-license-manager/licensecc/issues)
[![Forum](https://img.shields.io/badge/forum-licensecc-blue.svg?style=flat)](https://github.com/open-license-manager/licensecc/discussions)
[![Maintenance](https://img.shields.io/badge/maintenance-seeking--maintainers-yellow)](http://open-license-manager.github.io/licensecc/v2.1.0/other/maintenance.html)

Protect the software you develop from unauthorized copies, limit the usage in time, to a specific set of 
machines, or prevent the usage in  virtualized environments. It is an open source license library that helps to keep your software closed :smirk: . Among other features: it can generate a signature of that hardware it is running on and later report if the signature doesn't match (because the executable has been moved elsewhere).

A comprehensive [list of features](http://open-license-manager.github.io/licensecc/v2.1.0/analysis/features.html), and their status is available in the project site. 

If you're experiencing problems, or you need information you can't find in the **[documentation](http://open-license-manager.github.io/licensecc/v2.1.0/index.html)** please contact us on [github discussions](https://github.com/open-license-manager/licensecc/discussions).

## License

Distributed under the [BSD 3-Clause License](LICENSE). It comes with freedom of use for everyone, and it always will be.

## Should I use it?

The project is fully open source with no hidden costs or premium tiers.

**Good fit if:**
- You have a small to medium size application to protect
- Your user base is limited (10-20 customers), and you can distribute licenses manually
- Your project already uses CMake

**Consider alternatives if:**
- You need commercial support or guaranteed response times — see [maintenance status](http://open-license-manager.github.io/licensecc/v2.1.0/other/maintenance.html)
- You need a full license management system for distributing and renewing licenses at scale

## Project Structure

The software is made by 4 main sub-components:
-   a C++ library with a nice C api, `licensecc` with minimal (or no) external dependencies (the part you have to integrate in your software) that is the project you're currently in.
-   a license debugger `lcc-inspector` to be sent to the final customer when there are licensing problems or for calculating the pc hash before issuing the license.
-   a license generator (github project [lcc-license-generator](https://github.com/open-license-manager/lcc-license-generator)) `lccgen` for customizing the library and generate the licenses.
-   Usage [examples](https://github.com/open-license-manager/examples) to simplify the integration in your project.

## How to start

* Read the [quickstart](http://open-license-manager.github.io/licensecc/v2.1.0/usage/quickstart.html) and [concepts](http://open-license-manager.github.io/licensecc/v2.1.0/usage/concepts.html) sections in the documentation. This is the fastest way to get acquainted with the project.
* The [examples](https://github.com/open-license-manager/examples) repository that shows various ways to integrate `licensecc` into your project.

## How to build

You can find detailed instructions for [Linux](http://open-license-manager.github.io/licensecc/v2.1.0/development/Build-the-library.html) 
or [Windows](http://open-license-manager.github.io/licensecc/v2.1.0/development/Build-the-library-windows.html) in the project web site. 

### Prerequisites

-   Operating system: Ubuntu 22.04 to 26.04 (tested), other may be supported. Windows server 2022
-   compilers       : GCC (Linux), Visual Studio 2022 or 2026 (Windows) 
-   tools           : cmake(>=3.16), git, make/ninja
-   libs            : If target is Linux Openssl is required. Windows depends only on system libraries. Boost is necessary to build license generator and to run the tests but it's NOT a dependency of the final `licensecc` library. 

For a complete list of dependencies and supported environments see [the project website](http://open-license-manager.github.io/licensecc/v2.1.0/development/Dependencies.html)

### Configure & compile
Clone the project. It has submodules, don't forget the `--recursive` option.

```console
git clone --recursive https://github.com/open-license-manager/licensecc.git
cd licensecc/build
```

### build on Linux

```console
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make
make install
```

### build on Windows (with Visual Studio 2026 or Visual Studio Code)

* check out the project with git.
* go to Project -> cmake settings for licensecc -> Add -DBoost_ROOT="C:\path\to\boost" to the cmake command line arguments. 
* Check that boost is found and build using the UI.

## How to use

The [examples](https://github.com/open-license-manager/examples) repository that shows various ways to integrate `licensecc` into your project.

## How to contribute

Use [discussions](https://github.com/open-license-manager/licensecc/discussions) for questions and help. Open an [issue](https://github.com/open-license-manager/licensecc/issues) for bug reports and feature requests. 
See the [contribution guidelines](CONTRIBUTING.md) before reporting.

There are infinite ways you can contribute. A special thanks to `Quaoar Workshop` for creating [a video](https://youtu.be/BHb-p-4aaZE?si=pdlkgZX5WmWivhwb) about this library. Please note that they independently produced this content (we are not affiliated with them). BTW the video provides a nice introduction if you want to get started.