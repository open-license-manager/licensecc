# licensecc v2.1.0 Release Notes

## Breaking Change: Licenses Not Backward Compatible

**Licenses issued with v2.0.0 will NOT validate in v2.1.0.** Regenerate and re-issue all licenses if you need to upgrade (otherwise download tag v2.0.5, still has some build fixes and hardware identifiers are backward compatible):

- **Hardware identifier changes** — PC-identifier generation was reworked for consistency, so old hardware-bound licenses fail signature/identifier validation.
- **Key size improvements** — native support for **4096-bit RSA keys** on Linux and Windows; project generation switched to 2048-bit keys by default, the new keys are not compatible with version 2.0.0.

## Main Changes

1. **4096-bit RSA key support** on Linux (`#165`) and Windows (`#186`), with updated generator defaults.
2. **Reworked hardware identifiers** — disk identification now uses the real physical serial number + UUID, with fallback to legacy behavior (`#145`, `#199`).
3. **New identification strategies** — `CpuModelStrategy` (`#134`) and a system-id strategy with OS-specific identifier detection, selectable via dependency-injection refactoring.
4. **ARM / ARM64 support** — comprehensive CPU detection for ARM, ARM64, and NVIDIA Jetson, including ARM hypervisor/VM detection (`#83`).
5. **Execution-environment detection** — improved on-premise vs. cloud (Azure) detection.
6. **Improved `lcc-inspector`** — added CPU-cores detection, fixed a license signature mismatch on Windows.
7. **Customizable `LicenseLocator`s** — locators are now clonable and injectable, allowing integrators to plug in custom license-discovery logic.
8. **Larger default proprietary-data buffer** (`#198`) and fixed identifier buffer sizing.
9. **Modernized build & CI** — GitHub Actions replaces Travis, OpenSSL up to **4.0.x** supported, Boost 1.65–1.90 compatibility, optional shared-library build.
10. **Docs & quality** — version selector (`#188`), removed `cout`/`cerr` debug output, `noexcept` verification path, fixed base64 decoding and Windows signature-check edge cases.

