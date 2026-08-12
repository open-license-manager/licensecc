# Project Compilation Skill

## Overview

This skill provides instructions for configuring and compiling the licensecc project.

## Configuration Steps

Below: 
${ workspace } is the folder where the project has been checked out.
${ build-dir } is "${workspace}/build" the folder where the project is built

### 1. Clean
Run this step to clean the project.

```bash
cd ${ build-dir } && rm -rf *;
```

### 2. Configure with CMake

```bash
cd ${ build-dir } && rm -rf CMakeCache.txt; cmake -S ${ workspace } -B ${ build-dir }
```

Or with specific options:
```bash
${ build-dir } && && rm -rf CMakeCache.txt; cmake -S ${ workspace } -B ${ build-dir } -DSTATIC_RUNTIME=ON
```

### 3. Build the Project

```bash
cd ${ build-dir } && cmake --build . -j 8
```

## Build Options


### Static Runtime Linking
- `-DSTATIC_RUNTIME=ON` (default): Creates statically linked executable
- `-DSTATIC_RUNTIME=OFF`: Creates dynamically linked executable

### OpenSSL Configuration

- On Linux: OpenSSL is mandatory and will be automatically detected
- On Windows: OpenSSL is optional, will fall back to Windows Crypto API if not found. 
  - If OpenSSL found but still want to compile without it use `-DUSE_OPENSSL=OFF`

### Boost Configuration

- Boost libraries are automatically detected
- Required components: date_time, filesystem, program_options, unit_test_framework

## Testing

After building, run tests with:
```bash
ctest ${ build-dir }
```

## Troubleshooting

- If the project does not compile try to do the clean and configuration step again.
- The configure step is necessary if any of the CMakeLists.txt is changed.
- If OpenSSL is not found on Linux, install OpenSSL development packages

### modifications to licensecc_properties.h.in

when you modify licensecc_properties.h.in it is necessary to 
```bash
rm 'projects/DEFAULT/include/licensecc_properties.h'
```
and run the configuration step.

### modification to private keys

when you modify private or public key generation classes or you update
licensecc-generator you need to clean up the project folder 
```bash
rm -Rf 'projects/*'
```
and run the configuration step again.

