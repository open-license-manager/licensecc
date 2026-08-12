# Copilot Instructions for licensecc

## Project Overview

`licensecc` is a C/C++ licensing and copy-protection library for Windows and Linux.
It provides hardware-locked licenses, time-limited licenses, and a minimal-dependency
C API that host applications link against. This repo builds **only** the core library
(`liblicensecc`) and the `lcc-inspector` diagnostic tool.

The license *generator* (`lccgen`) lives in a separate repository,
[`lcc-license-generator`](https://github.com/open-license-manager/lcc-license-generator),
checked out here as the `extern/license-generator` git submodule. Its classes
(`License`, `CommandLineParser`, `CryptoHelper`, `CryptoHelperWindows`,
`CryptoHelperLinux`, …) are **not part of this repo** — do not suggest edits to
`extern/license-generator` unless the user is explicitly working on the generator;
that submodule has its own instructions upstream.

## Security sensitivity

This is a copy-protection library — its entire job is to be hard to bypass. Treat
`src/library/limits/`, `src/library/os/*/signature_verifier*`, and anything touching
RSA/SHA256 verification as security-critical:
- Never weaken, bypass, or add a way to skip signature/limit checks, even temporarily
  for "testing" — flag it to the user instead.
- Never log private keys, license file contents, or raw signatures.
- Don't change the public-key handling or the wire format of license files without
  the user explicitly asking for it.

## Architecture (this repo)

- `include/licensecc/` — public C API headers (`licensecc.h`, `datatypes.h`).
- `src/library/licensecc.cpp` — thin C API surface. `identify_pc` and `acquire_license`
  just delegate to `LicenseFacade`. **Do not change these function signatures.**
- `src/library/license_facade.{hpp,cpp}` — `LicenseFacade`: the actual entry point that
  coordinates locating, parsing, and verifying licenses. Most new logic belongs here or
  in the components it calls, not in `licensecc.cpp`.
- `src/library/LicenseParser.{hpp,cpp}` — reads/parses license `.ini` files (class is
  `LicenseParser`, *not* `LicenseReader`).
- `src/library/limits/license_verifier.{hpp,cpp}` — `LicenseVerifier`: signature and
  limit checking.
- `src/library/hw_identifier/` — `HwIdentifierFacade` + `IdentificationStrategy`
  subclasses (Ethernet, disk, default) that generate the PC signature.
- `src/library/locate/` — strategies for finding a license (file, env var, application
  folder, external definition).
- `src/library/os/` — platform-specific code, split into `windows/`, `linux/`, and
  `openssl/`. Keep platform code behind the existing `os.h` / interface headers rather
  than adding new `#ifdef`s inline elsewhere.
- `src/library/base/` — logging, base64, string/file utils, `EventRegistry`.
- `src/library/ini/` — vendored `SimpleIni` — external code, do not modify.
- `src/inspector/` — `lcc-inspector`, the diagnostic CLI sent to customers /
  used to compute a PC hash before issuing a license.
- `projects/<NAME>/include/licensecc/<NAME>/public_key.h` — generated at first build from the configured signing key. Never hand-edit; it doesn't exist until you configure the project.
- `projects/<NAME>/include/licensecc/<NAME>/licensecc_properties.h` — generated at first build. Never hand-edit; it doesn't exist until you configure the project. If you need to modify it, edit `src/templates/licensecc_properties.h.in`, `rm licensecc_properties.h` and re-run the project configuration step.
- `test/library/` — unit tests (Boost.Test) for the pieces above. `test/functional/`
  — end-to-end tests that generate a real license and verify it.
- `doc/` — Sphinx docs; `doc/analysis/features.html` tracks feature status, check it
  before assuming a feature is unimplemented.

## Build & test

Quick procedure (if no errors are expected):
```
cmake -S . -B build [-DSTATIC_RUNTIME=OFF] [-DUSE_OPENSSL=ON]
cmake --build build -j 8
ctest --test-dir build
```

Full procedure if quick prcedure fails: avoid using Build_CMakeTools or RunCtest_CMakeTools - strictly follow COMPILE.md instructions , use Compiler agent defined in this project: `.agents/skills/COMPILE.md`. 

- OpenSSL is **mandatory on Linux**, optional on Windows (falls back to Windows Crypto API / BCrypt).
- Boost (date_time, filesystem, program_options, unit_test_framework) is needed only
  for tests and the generator submodule — never make it a dependency of `liblicensecc` itself.
- CI matrices: `.github/workflows/linux-standard.yml`,
  `windows-standard.yml`, `windows-arm.yml` — check these before assuming a platform
  is or isn't covered.
- For non-trivial build/compile/fix-errors work, use the `Compiler` subagent
  (`.github/agents/compile-and-test.agent.md`) instead of ad-hoc shell commands.

## Coding standards

C++ RAII / value-semantics / const-correctness rules and common pitfalls
(dangling `.c_str()`, temporaries bound to references, etc.) are enforced via
File Instructions that auto-attach to C++ files:
`.github/instructions/coding-guidelines.instructions.md`.

- Format with the repo's `.clang-format` before finishing any edit; don't reformat
  unrelated code (see `CONTRIBUTING.md`).
- C++11 is the language standard — don't introduce newer-standard-only features.
- Target branch for PRs is `develop` (GitFlow), from a feature branch — never suggest
  committing straight to `develop` or `master`.

## Common tasks (this repo)

- New license field: extend `LicenseParser` / the values it reads, plus the
  `LicenseInfo` struct in `include/licensecc/datatypes.h`.
- New hardware-ID method: add an `IdentificationStrategy` subclass in
  `src/library/hw_identifier/` and register it in `HwIdentifierFacade`.
- New license-location source: add a strategy in `src/library/locate/`.
- Platform-specific behavior: add to `src/library/os/{windows,linux}/`, keep the
  cross-platform header the single point of contact.
- New tests: `test/library/` for units, `test/functional/` for end-to-end
  generate-and-verify flows; both use Boost.Test and register in the local
  `CMakeLists.txt`.

## Interaction with the user

- Audience is skilled. Be concise in explanations.
