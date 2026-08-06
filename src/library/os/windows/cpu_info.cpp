/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#include <string>
#include <unordered_set>
#include <cstring>
#include "../cpu_info.hpp"

#ifdef _M_ARM64
#include <windows.h>
#include <winreg.h>
#else
#include <intrin.h>
#endif

namespace license {
namespace os {
using namespace std;

#ifdef _M_ARM64 || defined(__aarch64__) || defined(__arm__)

static string get_cpu_vendor() {
	// On ARM64, get CPU vendor from registry
	HKEY hKey;
	LONG lRes =
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	string result = "Unknown";
	if (lRes == ERROR_SUCCESS) {
		wchar_t identifier[256] = {0};
		DWORD dataSize = sizeof(identifier);

		// Try to get the identifier which contains vendor info
		lRes = RegQueryValueExW(hKey, L"Identifier", NULL, NULL, (LPBYTE)identifier, &dataSize);
		if (lRes == ERROR_SUCCESS) {
			// Convert wide string to regular string
			int len = wcslen(identifier);
			result.resize(len);
			for (int i = 0; i < len; ++i) {
				result[i] = static_cast<char>(identifier[i]);
			}
		} else {
			// If identifier not available, try VendorIdentifier
			lRes = RegQueryValueExW(hKey, L"VendorIdentifier", NULL, NULL, (LPBYTE)identifier, &dataSize);
			if (lRes == ERROR_SUCCESS) {
				int len = wcslen(identifier);
				result.resize(len);
				for (int i = 0; i < len; ++i) {
					result[i] = static_cast<char>(identifier[i]);
				}
			}
		}
		RegCloseKey(hKey);
	}
	return result;
}

static string get_cpu_brand() {
	// On ARM64, get CPU brand from registry
	HKEY hKey;
	LONG lRes =
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	string result = "Unknown";
	if (lRes == ERROR_SUCCESS) {
		wchar_t processorName[256] = {0};
		DWORD dataSize = sizeof(processorName);

		// Fetch ProcessorNameString (e.g., "Snapdragon(TM) X Elite...")
		lRes = RegQueryValueExW(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)processorName, &dataSize);
		if (lRes == ERROR_SUCCESS) {
			// Convert wide string to regular string
			int len = wcslen(processorName);
			result.resize(len);
			for (int i = 0; i < len; ++i) {
				result[i] = static_cast<char>(processorName[i]);
			}
		} else {
			result = "ARM Processor";
		}
		RegCloseKey(hKey);
	}
	return result;
}
bool CpuInfo::is_virtual() const { return false; }

uint32_t CpuInfo::model() const {
	// On ARM64, we can get model information from registry
	HKEY hKey;
	LONG lRes =
		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	uint32_t result = 0;
	if (lRes == ERROR_SUCCESS) {
		DWORD modelNumber = 0;
		DWORD dataSize = sizeof(modelNumber);

		// Try to get the Architecture value
		lRes = RegQueryValueExW(hKey, L"~MACHINEHALNAME", NULL, NULL, (LPBYTE)&modelNumber, &dataSize);
		if (lRes != ERROR_SUCCESS) {
			// If not available, try Architecture value
			lRes = RegQueryValueExW(hKey, L"Architecture", NULL, NULL, (LPBYTE)&modelNumber, &dataSize);
			if (lRes != ERROR_SUCCESS) {
				modelNumber = 0;  // Default value if not found
			}
		}
		result = static_cast<uint32_t>(modelNumber);
		RegCloseKey(hKey);
	}
	return result;
}

bool CpuInfo::virt_info_available() const { return false; }

#elif defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || \
	defined(_M_IX86)
static string get_cpu_vendor() {
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	int cpui[4] = {0};
	__cpuidex(cpui, 0x0, 0x0);

	char vendor[13];
	memset(vendor, 0, sizeof(vendor));
	*reinterpret_cast<int*>(vendor) = cpui[1];
	*reinterpret_cast<int*>(vendor + 4) = cpui[3];
	*reinterpret_cast<int*>(vendor + 8) = cpui[2];
	return string(vendor, 12);
}

uint32_t CpuInfo::model() const {
	int cpui[4] = {0};
	__cpuid(cpui, 0x1);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (cpui[0] & 0x3FFF) | (cpui[0] & 0x3FF8000) >> 2 | (cpui[1] & 0xff) << 24;
}

static string get_cpu_brand() {
	int cpui[4] = {0};
	__cpuid(cpui, 0x80000000);
	int maxSupported = cpui[0];

	char brand[0x41];
	memset(brand, 0, sizeof(brand));
	string result;
	if (maxSupported >= 0x80000004) {
		int instruction = 0x80000002;
		for (int i = 0; i <= 2; ++i) {
			__cpuidex(cpui, instruction + i, 0);
			memcpy(&brand[i * sizeof(cpui)], cpui, sizeof(cpui));
		}
		result = string(brand);
	} else {
		result = "NA";
	}

	return result;
}

/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::is_virtual() const {
	int cpui[4] = {0};
	__cpuid(cpui, 0x1);

	return ((cpui[2] >> 31) & 1);
}

bool CpuInfo::virt_info_available() const { return true; }
#endif

CpuInfo::CpuInfo() : m_vendor(get_cpu_vendor()), m_brand(get_cpu_brand()) {}

CpuInfo::~CpuInfo() {}

}  // namespace os
} /* namespace license */
