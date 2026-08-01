# Copilot Instructions for lcc-license-generator

## Project Overview

C/C++ licensing library for copy protection and licensing management on Windows and Linux. Provides hardware-locked licensing, time-limited licenses, and copy protection features. The library allows software developers to protect their applications from unauthorized use by verifying licenses tied to specific hardware identifiers. Includes a C API with minimal dependencies for easy integration into existing software projects.

## Key Components

- **License Verification**: Validates licenses against hardware identifiers and time limits
- **Hardware Identification**: Generates unique PC signatures using various strategies (MAC address, disk info, IP, etc.)
- **License Reader**: Reads and parses license files from various locations
- **License Verifier**: Verifies license signatures and checks compliance with license terms
- **Inspector Tool**: Diagnostic utility for troubleshooting licensing issues and calculating hardware identifiers

## Architecture

- `src/library/` - Core licensing functionality and C API implementation
- `src/inspector/` - Inspector tool for diagnostics and hardware identification
- `src/library/hw_identifier/` - Hardware identification strategies and PC signature generation
- `src/library/limits/` - License verification and limit checking logic
- `src/library/locate/` - License location strategies (file, environment variable, etc.)
- `src/library/os/` - OS-specific implementations and system information gathering
- `src/library/ini/` - INI file parsing for license files. External library do not change.
- `test/` - Unit tests using Boost.Test framework
- `cmake/` - CMake modules and build configurations
- `include/licensecc/` - Public API headers
- `src/library/licensecc.cpp` Methods: `acquire_license` and `identify_pc` public API implementation, do not change the signatures or their parameters.
- `projects/DEFAULT/include/licensecc/DEFAULT/public_key.h` public key to validate license signatures. It is generated during the first build process and should not be modified.
- `doc/` - Documentation and usage guides

## Key Classes

- `HwIdentifierFacade` - Main interface for generating hardware identifiers using different strategies
- `IdentificationStrategy` - Abstract class for hardware identification strategies with concrete implementations (Ethernet, Disk, IP, etc.)
- `LicenseReader` - Reads and parses license files from various sources
- `LicenseVerifier` - Verifies license signatures and validates license compliance
- `LicenseInfo` - Data structure representing license information and status
- `EventRegistry` - Tracks and manages licensing events and errors

## Build Configuration

- C++11 standard required
- OpenSSL support (mandatory on Linux, optional on Windows)
- Uses Boost libraries only for test. Not a deplendency of the library.

## Cryptographic Features

- RSA key pairs (default 2048-bit, configurable)
- SHA256withRSA signature algorithm
- Base64 encoding for signatures
- Support for both OpenSSL and Windows Crypto API
- Private keys in OpenSSL format, public keys in binary PKCS1 encoded format (build/)


## Coding Standards

See [CODING_GUIDELINES.md](CODING_GUIDELINES.md) for detailed coding standards, common pitfalls for developers coming from other languages, and specific C++ best practices.

## Common Tasks

- Adding new license parameters: Extend the License class and its values_map
- Modifying cryptographic algorithms: Implement in CryptoHelper subclasses
- Adding command-line options: Update CommandLineParser class
- Creating new tests: Add to test/ directory using Boost.Test framework

### Compilation and test:

- run the subagent `compile-and-test.md`

## Interaction with the user

- Be very concise in explanations
- Do not output a summary changes