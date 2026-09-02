# Quickstart

This guide walks through the complete integration workflow step by step. 

`Licensecc` in the build process generates an rsa keypair that is stored in the file system.
Using the private key you will generate the licenses and distribute them to the users.
The public key is buried inside the library where (hopefully) nobody will find it.

This is why we don't distribute a pre-compiled version of the library. Each build contains the `secret` "public" key (though this may change in future).

The proposed workflow assumes you are using `cmake` in your project. If not you will start from step 2 and you will have to adapt the rest. 

## Workflow Overview

```
+----------------------------------------------+
|  Step 1: Add submodule + add_subdirectory()  |
|  in CMakeLists.txt                           |
+--------------------+-------------------------+
                     |
                     v
+----------------------------+  +----------------------------------------------------------+
|  Step 2: Launch cmake      |  |                    [licensecc build]                     |
|         build              |  |                                                          |
|                            |  |     +----------+     +----------+     +----------+       |
|                            |  |     | Step 3:  |     | Step 4:  |     | Step 5:  |       |
|                            |  |     | Build    +--->+| Init     +--->+| Compile  |       |
|                            |  |     | lccgen   |     | Project  |     | library  |       |
|                            |  |     +----------+     +----------+     +----------+       |
+----------------------------+  +----------------------------------------------------------+
                     |
                     v
+----------------------------+  +----------------------------+
|  Step 6: Integrate into    |  |  Step 7: Issue licenses    |
|  your application          |  |  (lccgen)                  |
+----------------------------+  +----------------------------+
```

## Step 1: Add submodule + configure CMake

Add `licensecc` as a submodule and include it in your build:

```console
cd /path/to/your/project
git submodule add https://github.com/open-license-manager/licensecc.git submodules/licensecc
git submodule update --init --recursive
```

The `--recursive` flag is required because `licensecc` itself contains submodules
(notably the license generator `lccgen`).

In your project's `CMakeLists.txt`, add the subdirectory:

```cmake
add_subdirectory(submodules/licensecc)
```

This makes the following targets available to your build:
- `licensecc::licensecc` -- the licensing library. *Add it to your application*
- `lccgen` -- the license generator executable (used internally by the build. Once it is built install it to a folder in your PATH to issue licenses)

See [Locate and link the library](integration.rst#step-1-locate-and-link-the-library) for more details.

## Step 2: Launch cmake build

Configure and build your project as usual:

```console
mkdir -p build && cd build
cmake .. -DLCC_PROJECT_NAME=MyApp
make
```

This triggers the full `licensecc` build pipeline (detailed in the next section).

---

## [licensecc build] Steps 3-5

The following steps happen automatically inside the `licensecc` build tree when
you run `cmake .. && make`. 

### Step 3: Build lccgen (License Generator)

The license generator is built first as a build dependency of the library.
It is compiled from the `extern/license-generator/` submodule.

Output:
- `lccgen` -- the license generator executable

```
+---------------------+
|  extern/            |
|  license-generator  +-----> lccgen
|  (submodule)        |       
+---------------------+
```

### Step 4: Initialize Your Project

After `lccgen` is built, CMake runs it to generate the project key pair and
headers. The project name is set via `-DLCC_PROJECT_NAME=MyApp`.

This creates the following structure:

```
projects/
└── MyApp/
    ├── include/
    │   └── licensecc/
    │       └── MyApp/
    │           ├── licensecc_properties.h   # Project customizations **This is an important customization point**
    │           └── public_key.h             # Public key (it will be embedded in `licensecc`)
    ├── private_key.rsa                      # Private key (keep secret!)
    └── licenses/                            # The license files you will generate
```

```
+-------------------+       +-------------------+
|  lccgen \         +------>+  projects/MyApp/  |
|  project init     |       |                   |
|  (cmake build)    |       |  + public_key.h   |
|                   |       |  + private_key.rsa|
+-------------------+       +-------------------+
```

> **Warning**: The private key (`private_key.rsa`) must be kept secret. Anyone with access
> to it can issue valid licenses for your software. The public key is compiled into the
> library and distributed with your application.

### Step 5: Compile the Library

The library is compiled with your public key embedded. The static library
`liblicensecc_static.a` (or `licensecc_static.lib` on Windows) is produced.

```
+-------------------+       +---------------------------+
|  projects/MyApp/  |       |  liblicensecc_static.a    |
|                   |       |                           |
|  + public_key.h   +------>+  (public_key.h embedded)  |
|  + properties     |       |  (verification logic)     |
+-------------------+       +---------------------------+
```

The library is installed to the CMake install prefix:

```
install/
├── include/
│   └── licensecc/
│       ├── licensecc.h                # Public C API
│       ├── Licensecc.hpp              # Public C++ API
│       ├── LocatorStrategy.hpp
│       ├── datatypes_cpp.hpp          # C++ data types (FullLicenseInfo, LimitVerifierFn)
│       ├── EventRegistry.h
│       └── MyApp/
│           ├── licensecc_properties.h
│           └── public_key.h
└── lib/
    └── MyApp/
        ├── liblicensecc_static.a
        └── cmake/
            └── licenseccConfig.cmake
```

## Step 6: Integrate into Your Application

### Using CMake (recommended)

Since you added `add_subdirectory(submodules/licensecc)`, you can link directly:

```cmake
target_link_libraries(my_app PRIVATE licensecc::licensecc)
```

No `find_package` or `Findlicensecc.cmake` is needed when using the submodule approach.

### API Usage

```c
#include <licensecc/licensecc.h>

/* Acquire and verify a license */
LicenseInfo info;
LCC_EVENT_TYPE result = acquire_license(NULL, NULL, &info);

if (result == LICENSE_OK) {
    /* License is valid -- proceed */
} else {
    /* Handle license error */
}
```

C++ applications can use the C++ facade instead:

```cpp
#include <licensecc/Licensecc.hpp>

license::Licensecc licensecc;
LCC_EVENT_TYPE result = licensecc.acquire_license(nullptr, nullptr, &info);
```

See [Step 6 — Call Licensecc from your code](integration.rst#step-6-call-licensecc-from-your-code) for a detailed integration example with error handling.

```
+-------------------+       +-------------------+
|  Your App         |       |  licensecc        |
|  (CMake project)  +------>+  static library   |
|                   |       |                   |
|  main.cpp         |       |  public_key.h     |
|  CMakeLists.txt   |       |  verification     |
+-------------------+       +-------------------+
```


## Step 7: Issue Licenses

License generation is described in detail in [License Generation](issue-licenses.md). 
Below a quick summary of what you can do.

Use `lccgen` to sign and issue license files. Navigate to the project directory:

```console
cd projects/MyApp
```

### Time-limited license (30-day demo)

```console
lccgen license issue --expiry-date 30 -o licenses/my_app_demo.lic
```

### Hardware-locked license

First, obtain the hardware signature from the target machine (run `lccinspector` or
your app calling `identify_pc()`), then:

```console
lccgen license issue \
    --client-signature XXXX-XXXX-XXXX-XXXX \
    -o licenses/my_app_hw.lic
```

```
+-------------------+       +-------------------+
|  projects/MyApp/  |       |  License File     |
|                   |       |                   |
|  private_key.rsa  +------>+  my_app.lic       |
|  (signing key)    |       |  (signed output)  |
+-------------------+       +-------------------+
```

Place the generated `.lic` file in the application's expected license location
(see [License Retrieval](find-the-license.md) for supported locations).

