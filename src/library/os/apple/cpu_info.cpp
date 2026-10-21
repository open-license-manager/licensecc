/*
 * cpu_info.cpp - macOS implementation
 *
 * Uses sysctl(3) and, on x86, the cpuid instruction. No subprocesses.
 */

#include "../cpu_info.hpp"
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/sysctl.h>

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#endif

namespace license {
namespace os {
using namespace std;

static int get_cpu_cores_value() { return static_cast<int>(sysconf(_SC_NPROCESSORS_CONF)); }
static int get_max_cpu_value() { return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN)); }

/**
 * Read a sysctl string parameter, e.g. "machdep.cpu.brand_string".
 */
static string sysctl_string(const char* name) {
	string result;
	size_t size = 0;
	if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0) {
		return result;
	}
	result.resize(size);
	if (sysctlbyname(name, &result[0], &size, nullptr, 0) != 0) {
		return "";
	}
	while (!result.empty() && result.back() == '\0') {
		result.pop_back();
	}
	return result;
}

#if defined(__x86_64__) || defined(__i386__)
// x86/x64 implementation (same technique as the Linux backend)

struct CPUVendorID {
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	string toString() const { return string(reinterpret_cast<const char*>(this), 12); }
};

static string get_cpu_vendor() {
	unsigned int level = 0, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	CPUVendorID vendorID;
	vendorID.ebx = ebx;
	vendorID.edx = edx;
	vendorID.ecx = ecx;
	return vendorID.toString();
}

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
 * Detect virtual machine using the hypervisor bit.
 */
bool CpuInfo::is_virtual() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);

	bool is_virtual = (((ecx >> 31) & 1) == 1);	 // hypervisor flag
	return is_virtual;
}

bool CpuInfo::virt_info_available() const { return true; }

uint32_t CpuInfo::model() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (eax & 0x3FFF) | (eax & 0x3FF8000) >> 2 | (ebx & 0xff) << 24;
}

#else
// Apple Silicon (ARM) implementation

CpuInfo::CpuInfo()
	: m_vendor("Apple"),
	  m_brand(sysctl_string("machdep.cpu.brand_string")),
	  m_cpu_cores(get_cpu_cores_value()),
	  m_max_cpu(get_max_cpu_value()) {}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const { return 0; }

/**
 * On Apple Silicon the CPUID hypervisor bit is not available. Guest VMs
 * report a distinctive "hw.model" (e.g. "Parallels Virtual Platform",
 * "Apple Virtual Machine", "VMware7,1"); bare metal reports a Mac model
 * identifier like "MacBookPro18,3" or "Mac14,2".
 */
bool CpuInfo::is_virtual() const {
	const string hw_model = sysctl_string("hw.model");
	const string brand = this->brand();
	static const char* vm_hints[] = {"virtual", "parallels", "vmware", "virtualbox", "qemu", "bhyve"};
	for (const char* hint : vm_hints) {
		if (hw_model.find(hint) != string::npos || brand.find(hint) != string::npos) {
			return true;
		}
	}
	return false;
}

bool CpuInfo::virt_info_available() const { return false; }

#endif

}  // namespace os
} /* namespace license */
