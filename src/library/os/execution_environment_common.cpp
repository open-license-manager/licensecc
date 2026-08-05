/*
 *
 *  Created on: Feb 23, 2020
 *      Author: GC
 */

#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <array>
#include <licensecc/datatypes.h>

#include "../base/base.h"
#include "cpu_info.hpp"
#include "execution_environment.hpp"

namespace license {
namespace os {
using namespace std;

const unordered_map<string, LCC_API_VIRTUALIZATION_DETAIL> virtual_cpu_names{
	{"bhyve bhyve ", V_OTHER},	 {"KVM", KVM},		 {"MICROSOFT", HV},		  {" lrpepyh vr", HV},
	{"prl hyperv  ", PARALLELS}, {"VMWARE", VMWARE}, {"XenVMMXenVMM", V_XEN}, {"ACRNACRNACRN", V_OTHER},
	{"VBOX", VIRTUALBOX}};

const unordered_map<string, LCC_API_VIRTUALIZATION_DETAIL> vm_vendors{
	{"VMWARE", VMWARE},			  {"MICROSOFT", HV},	{"PARALLELS", PARALLELS}, {"VITRUAL MACHINE", V_OTHER},
	{"INNOTEK GMBH", VIRTUALBOX}, {"POWERVM", V_OTHER}, {"BOCHS", V_OTHER},		  {"KVM", KVM}};

const string pc_vendors[] = {"HEWLETT-PACKARD", "COMPAQ",  "DELL",	"ASUS",		"LENOVO", "ACER",
							 "TOSHIBA",			"RAZER",   "APPLE", "HP",		"LG",	  "ASROCK",
							 "GIGABYTE",		"BIOSTAR", "NZXT",	"FRAMEWORK"};

const string pc_vendors[] = {"HEWLETT-PACKARD", "COMPAQ",  "DELL",	"ASUS",		"LENOVO", "ACER",
							 "TOSHIBA",			"RAZER",   "APPLE", "HP",		"LG",	  "ASROCK",
							 "GIGABYTE",		"BIOSTAR", "NZXT",	"FRAMEWORK"};

static LCC_API_VIRTUALIZATION_DETAIL find_in_map(const unordered_map<string, LCC_API_VIRTUALIZATION_DETAIL>& map,
												 const string& data) {
	for (auto it : map) {
		if (data.find(it.first) != string::npos) {
			return it.second;
		}
	}
	return BARE_TO_METAL;
}

LCC_API_VIRTUALIZATION_SUMMARY ExecutionEnvironment::virtualization() const {
	LCC_API_VIRTUALIZATION_SUMMARY result;
	bool isContainer = is_container();
	if (isContainer) {
		result = LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER;
	} else if (virtualization_detail() != BARE_TO_METAL || is_cloud()) {
		result = LCC_API_VIRTUALIZATION_SUMMARY::VM;
	} else {
		result = LCC_API_VIRTUALIZATION_SUMMARY::NONE;
	}
	return result;
}

LCC_API_VIRTUALIZATION_DETAIL ExecutionEnvironment::virtualization_detail() const {
	LCC_API_VIRTUALIZATION_DETAIL result = BARE_TO_METAL;
	const string bios_description = m_board_info.bios_description();
	const string bios_vendor = m_board_info.bios_vendor();
	const string sys_vendor = m_board_info.sys_vendor();
	if ((result = find_in_map(vm_vendors, bios_description)) == BARE_TO_METAL) {
		if ((result = find_in_map(vm_vendors, bios_vendor)) == BARE_TO_METAL) {
			if ((result = find_in_map(vm_vendors, sys_vendor)) == BARE_TO_METAL) {
				if ((result = find_in_map(virtual_cpu_names, m_cpu_info.vendor())) == BARE_TO_METAL) {
					result = find_in_map(virtual_cpu_names, m_cpu_info.brand());
				}
			}
		}
	}
	if (result == BARE_TO_METAL) {
		result = guess_virtualization_by_os_quirks();
	}

	if (result == BARE_TO_METAL) {
		if (m_cpu_info.is_virtual() || is_cloud()) {
			result = V_OTHER;
		}
	}
	return result;
}

bool ExecutionEnvironment::is_cloud() const {
	const LCC_API_CLOUD_PROVIDER prov = cloud_provider();
	return prov != ON_PREMISE && prov != PROV_UNKNOWN;
}

LCC_API_CLOUD_PROVIDER ExecutionEnvironment::cloud_provider() const {
	LCC_API_CLOUD_PROVIDER result = PROV_UNKNOWN;
	const string bios_description = m_board_info.bios_description();
	const string bios_vendor = m_board_info.bios_vendor();
	const string sys_vendor = m_board_info.sys_vendor();
	if (bios_description.size() > 0 || bios_vendor.size() > 0 || sys_vendor.size() > 0) {
		if (bios_vendor.find("SEABIOS") != string::npos || bios_description.find("ALIBABA") != string::npos ||
			sys_vendor.find("ALIBABA") != string::npos) {
			result = ALI_CLOUD;
		} else if (sys_vendor.find("GOOGLE") != string::npos ||
				   bios_description.find("GOOGLECOMPUTEENGINE") != string::npos) {
			result = GOOGLE_CLOUD;
		} else if (bios_vendor.find("AWS") != string::npos || bios_description.find("AMAZON") != string::npos ||
				   sys_vendor.find("AWS") != string::npos) {
			result = AWS;
		} else if (bios_vendor.find("MICROSOFT CORPORATION") != string::npos ||
				   bios_description.find("MICROSOFTCORPORATION")) {
			result = AZURE_CLOUD;
		} else {
			for (const auto& vendor : pc_vendors) {
				if (sys_vendor.find(vendor) != string::npos) {
					result = ON_PREMISE;
				}
			}
		}
	}
	if (result == PROV_UNKNOWN) {
		result = guess_cloud_provider_by_os_quirks();
	}
	return result;
}
}  // namespace os
}  // namespace license
