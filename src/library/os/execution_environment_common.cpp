/*
 *
 *  Created on: Feb 23, 2020
 *      Author: GC
 */

#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <array>

#include "../base/base.h"
#include "cpu_info.hpp"
#include "execution_environment.hpp"

namespace license {
namespace os {
using namespace std;

VIRTUALIZATION ExecutionEnvironment::getVirtualization() const {
	VIRTUALIZATION result;
	CpuInfo cpuInfo;
	bool isContainer = is_container();
	if (isContainer) {
		result = CONTAINER;
	} else if (cpuInfo.cpu_virtual() || is_cloud()) {
		result = VM;
	} else {
		result = NONE;
	}
	return result;
}

bool ExecutionEnvironment::is_cloud() const { return getCloudProvider() != ON_PREMISE; }

// TODO test and azure
CLOUD_PROVIDER ExecutionEnvironment::getCloudProvider() const {
	CLOUD_PROVIDER result = PROV_UNKNOWN;
	if (bios_description.size() > 0 || bios_vendor.size() > 0 || sys_vendor.size() > 0) {
		if (bios_vendor.find("SEABIOS") != string::npos || bios_description.find("ALIBABA") != string::npos ||
			sys_vendor.find("ALIBABA") != string::npos) {
			result = ALI_CLOUD;
		} else if (sys_vendor.find("GOOGLE") != string::npos || bios_description.find("GOOGLE") != string::npos) {
			result = GOOGLE_CLOUD;
		} else if (bios_vendor.find("AWS") != string::npos || bios_description.find("AMAZON") != string::npos ||
				   sys_vendor.find("AWS") != string::npos) {
			result = AWS;
		}
	} 
	return result;
}
}  // namespace os
}  // namespace license
