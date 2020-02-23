/*
 * cpu_info_common.cpp
 *
 *  Created on: Jan 19, 2020
 *      Author: GC
 */
#include <unordered_map>
#include "cpu_info.hpp"

namespace license {
namespace os {
using namespace std;

const unordered_map<string, VIRTUALIZATION_DETAIL> virtual_cpu_names{
	{"bhyve bhyve ", V_OTHER}, {"KVMKVMKVM", KVM},		  {"Microsoft Hv", HV},
	{" lrpepyh vr", HV},	   {"prl hyperv  ", V_OTHER}, {"VMwareVMware", VMWARE},
	{"XenVMMXenVMM", V_XEN},   {"ACRNACRNACRN", V_OTHER}, {"VBoxVBoxVBox", VIRTUALBOX}};

/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::cpu_virtual() const {
	bool is_virtual = is_hypervisor_set();
	if (!is_virtual) {
		string cpu_vendor = vendor();
		auto it = virtual_cpu_names.find(cpu_vendor);
		is_virtual = (it != virtual_cpu_names.end());
	}
	return is_virtual;
}
VIRTUALIZATION_DETAIL CpuInfo::getVirtualizationDetail() const {
	string cpu_vendor = vendor();
	auto it = virtual_cpu_names.find(cpu_vendor);
	VIRTUALIZATION_DETAIL result = BARE_TO_METAL;
	if (it != virtual_cpu_names.end()) {
		result = it->second;
	} else if (is_hypervisor_set()) {
		result = (VIRTUALIZATION_DETAIL)V_OTHER;
	}
	return result;
}
}  // namespace os
}  // namespace license
