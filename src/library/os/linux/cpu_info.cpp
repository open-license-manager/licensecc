/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#include <cpuid.h>
#include <string>
#include <unordered_set>
#include "../cpu_info.hpp"

namespace license {
using namespace std;

struct CPUVendorID {
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;

	string toString() const { return string(reinterpret_cast<const char *>(this), 12); }
};

CpuInfo::CpuInfo() {}

CpuInfo::~CpuInfo() {}
/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::is_hypervisor_set() const {
	unsigned int level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);

	bool is_virtual = (((ecx >> 31) & 1) == 1);  // hypervisor flag
	return is_virtual;
}

uint32_t CpuInfo::model() const {
	unsigned int level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (eax & 0x3FFF) | (eax & 0x3FF8000) >> 2 | (ebx & 0xff) << 24;
}

string CpuInfo::vendor() const {
	unsigned int level = 0, eax = 0, ebx = 0, ecx = 0, edx = 0;
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	CPUVendorID vendorID{.ebx = ebx, .edx = edx, .ecx = ecx};
	return vendorID.toString();
}

} /* namespace license */
