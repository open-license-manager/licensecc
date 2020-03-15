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

bool ExecutionEnvironment::is_cloud() const {
	CLOUD_PROVIDER prov = getCloudProvider();
	return prov != ON_PREMISE && prov != PROV_UNKNOWN;
}

// TODO test and azure
CLOUD_PROVIDER ExecutionEnvironment::getCloudProvider() const {
	CLOUD_PROVIDER result = PROV_UNKNOWN;
	if (m_bios_description.size() > 0 || m_bios_vendor.size() > 0 || m_sys_vendor.size() > 0) {
		if (m_bios_vendor.find("SEABIOS") != string::npos || m_bios_description.find("ALIBABA") != string::npos ||
			m_sys_vendor.find("ALIBABA") != string::npos) {
			result = ALI_CLOUD;
		} else if (m_sys_vendor.find("GOOGLE") != string::npos || m_bios_description.find("GOOGLE") != string::npos) {
			result = GOOGLE_CLOUD;
		} else if (m_bios_vendor.find("AWS") != string::npos || m_bios_description.find("AMAZON") != string::npos ||
				   m_sys_vendor.find("AWS") != string::npos) {
			result = AWS;
		} else if (m_bios_description.find("HP-COMPAQ") != string::npos || m_bios_description.find("ASUS") ||
				   m_bios_description.find("DELL")) {
			result = ON_PREMISE;
		}
	}
	return result;
}
}  // namespace os
}  // namespace license
