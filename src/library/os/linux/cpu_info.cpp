/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#include "../cpu_info.hpp"
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <map>
#include <unistd.h>

// Include cpuid.h only for x86/x64 architectures
#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#include <memory.h>
#endif

namespace license {
namespace os {
using namespace std;

// Number of logical processors, obtained with OS calls (architecture independent).
static int get_cpu_cores_value() { return static_cast<int>(sysconf(_SC_NPROCESSORS_CONF)); }
static int get_max_cpu_value() { return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN)); }

#if defined(__x86_64__) || defined(__i386__)
// x86/x64 implementation

struct CPUVendorID {
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	string toString() const { return string(reinterpret_cast<const char*>(this), 12); }
};

static string get_cpu_vendor() {
	unsigned int level = 0, eax = 0, ebx = 0, ecx = 0, edx = 0;
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	CPUVendorID vendorID;
	vendorID.ebx = ebx;
	vendorID.edx = edx;
	vendorID.ecx = ecx;
	return vendorID.toString();
}

// https://en.wikipedia.org/wiki/CPUID
static string get_cpu_brand() {
	string result;
	uint32_t brand[0x10];

	if (!__get_cpuid_max(0x80000004, NULL)) {
		result = "NA";
	} else {
		memset(brand, 0, sizeof(brand));
		__get_cpuid(0x80000002, brand + 0x0, brand + 0x1, brand + 0x2, brand + 0x3);
		__get_cpuid(0x80000003, brand + 0x4, brand + 0x5, brand + 0x6, brand + 0x7);
		__get_cpuid(0x80000004, brand + 0x8, brand + 0x9, brand + 0xa, brand + 0xb);
		result = string(reinterpret_cast<char*>(brand));
	}
	return result;
}

CpuInfo::CpuInfo()
	: m_vendor(get_cpu_vendor()),
	  m_brand(get_cpu_brand()),
	  m_cpu_cores(get_cpu_cores_value()),
	  m_max_cpu(get_max_cpu_value()) {}

CpuInfo::~CpuInfo() {}

/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::is_virtual() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);

	bool is_virtual = (((ecx >> 31) & 1) == 1);	 // hypervisor flag
	return is_virtual;
}

/**
 * Check if virtualization information is available.
 * @return true for Windows and Linux Intel processors, false for ARM
 */
bool CpuInfo::virt_info_available() const { return true; }

uint32_t CpuInfo::model() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (eax & 0x3FFF) | (eax & 0x3FF8000) >> 2 | (ebx & 0xff) << 24;
}

#elif defined(__aarch64__) || defined(__arm__)
// ARM implementation

// ARM CPU implementer codes (from ARM Architecture Reference Manual)
static map<string, string> arm_vendors = {{"0x41", "ARM"},		{"0x42", "Broadcom"}, {"0x43", "Cavium"},
										  {"0x44", "DEC"},		{"0x4e", "NVIDIA"},	  {"0x50", "APM"},
										  {"0x51", "Qualcomm"}, {"0x53", "Samsung"},  {"0x54", "HiSilicon"},
										  {"0x56", "Marvell"},	{"0x69", "Intel"}};

static string get_arm_cpu_vendor() {
	ifstream cpuinfo("/proc/cpuinfo");
	string line;
	string vendor = "ARM";	// Default vendor

	while (getline(cpuinfo, line)) {
		if (line.find("CPU implementer") != string::npos) {
			size_t pos = line.find(":");
			if (pos != string::npos) {
				string implementer = line.substr(pos + 1);
				// Remove leading/trailing whitespace
				implementer.erase(0, implementer.find_first_not_of(" \t"));
				implementer.erase(implementer.find_last_not_of(" \t") + 1);

				auto it = arm_vendors.find(implementer);
				if (it != arm_vendors.end()) {
					vendor = it->second;
				}
			}
			break;
		}
	}

	// Check device tree for more specific info (e.g., NVIDIA Jetson)
	ifstream dt_compat("/proc/device-tree/compatible");
	if (dt_compat.is_open()) {
		string compat_str;
		getline(dt_compat, compat_str, '\0');  // Read null-terminated string
		if (compat_str.find("nvidia") != string::npos) {
			vendor = "NVIDIA";
		}
	}

	return vendor;
}

static string get_arm_cpu_brand() {
	string brand = "ARM Processor";

	// First try to get model name from cpuinfo
	ifstream cpuinfo("/proc/cpuinfo");
	string line;

	while (getline(cpuinfo, line)) {
		if (line.find("model name") != string::npos) {
			size_t pos = line.find(":");
			if (pos != string::npos) {
				brand = line.substr(pos + 1);
				// Remove leading/trailing whitespace
				brand.erase(0, brand.find_first_not_of(" \t"));
				brand.erase(brand.find_last_not_of(" \t") + 1);
			}
			break;
		}
	}

	// Try to get more specific info from device tree
	ifstream dt_model("/proc/device-tree/model");
	if (dt_model.is_open()) {
		string model;
		getline(dt_model, model);
		if (!model.empty() && model.find("nvidia") != string::npos) {
			// For NVIDIA Jetson devices, use the model from device tree
			brand = "NVIDIA " + model;
		} else if (!model.empty() && model != brand) {
			// Use device tree model if it's more specific
			brand = model;
		}
	}

	return brand;
}

static uint32_t get_arm_cpu_model() {
	uint32_t model = 0;
	ifstream cpuinfo("/proc/cpuinfo");
	string line;

	while (getline(cpuinfo, line)) {
		if (line.find("CPU part") != string::npos) {
			size_t pos = line.find(":");
			if (pos != string::npos) {
				string part_str = line.substr(pos + 1);
				// Remove leading/trailing whitespace
				part_str.erase(0, part_str.find_first_not_of(" \t"));
				part_str.erase(part_str.find_last_not_of(" \t") + 1);

				// Convert hex string to integer
				try {
					model = stoul(part_str, nullptr, 16);
				} catch (...) {
					model = 0;
				}
			}
			break;
		}
	}

	return model;
}

static bool is_arm_hypervisor() {
	// Check for common hypervisor indicators on ARM

	// 1. Check for hypervisor sysfs directory
	ifstream hypervisor("/sys/hypervisor/type");
	if (hypervisor.is_open()) {
		return true;
	}

	return false;
}

CpuInfo::CpuInfo()
	: m_vendor(get_arm_cpu_vendor()),
	  m_brand(get_arm_cpu_brand()),
	  m_cpu_cores(get_cpu_cores_value()),
	  m_max_cpu(get_max_cpu_value()) {}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const { return get_arm_cpu_model(); }

bool CpuInfo::is_virtual() const { return is_arm_hypervisor(); }

/**
 * Check if virtualization information is available.
 * @return true for Windows and Linux Intel processors, false for ARM
 */
bool CpuInfo::virt_info_available() const { return false; }

#else
// Generic/Unknown architecture fallback

CpuInfo::CpuInfo()
	: m_vendor("Unknown"),
	  m_brand("Unknown Processor"),
	  m_cpu_cores(get_cpu_cores_value()),
	  m_max_cpu(get_max_cpu_value()) {}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const { return 0; }

bool CpuInfo::is_virtual() const { return false; }

#endif

}  // namespace os
} /* namespace license */
