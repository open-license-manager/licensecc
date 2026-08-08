/*
 * execution_environment.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */

#include <paths.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <dirent.h>
#include <sys/utsname.h>

#include "../../base/base.h"
#include "../cpu_info.hpp"
#include "../execution_environment.hpp"
#include "../../base/file_utils.hpp"
#include "../../base/string_utils.h"

namespace license {
namespace os {
using namespace std;

// 0=NO 1=Docker/2=Lxc
static CONTAINER_TYPE checkContainerProc() {
	// in docer /proc/self/cgroups contains the "docker" or "lxc" string
	// https://stackoverflow.com/questions/23513045/how-to-check-if-a-process-is-running-inside-docker-container

	FILE* fp;
	char* line = nullptr;
	size_t len = 0;
	ssize_t read;
	CONTAINER_TYPE result = CONTAINER_TYPE::NONE;

	fp = fopen("/proc/self/cgroup", "r");
	if (fp == nullptr) {
		return CONTAINER_TYPE::NONE;
	}

	while ((read = getline(&line, &len, fp)) != -1 && result == CONTAINER_TYPE::NONE) {
		if (strstr(line, "docker") != NULL) {
			result = CONTAINER_TYPE::DOCKER;
		}
		if (strstr(line, "lxc") != NULL) {
			result = CONTAINER_TYPE::LXC;
		}
	}

	if (line) {
		free(line);
	}
	fclose(fp);
	return result;
}

// 0=NO 1=Docker/2=Lxc
static CONTAINER_TYPE checkSystemdContainer() {
	ifstream systemd_container("/var/run/systemd/container");
	CONTAINER_TYPE result = CONTAINER_TYPE::NONE;
	if (systemd_container.good()) {
		result = CONTAINER_TYPE::DOCKER;
		for (string line; getline(systemd_container, line);) {
			if (line.find("docker") != string::npos) {
				result = CONTAINER_TYPE::DOCKER;
				break;
			} else if (line.find("lxc") != string::npos) {
				result = CONTAINER_TYPE::LXC;
				break;
			}
		}
	}
	return result;
}

static CONTAINER_TYPE get_container_type() {
	CONTAINER_TYPE result = checkContainerProc();
	if (result == CONTAINER_TYPE::NONE) {
		result = checkSystemdContainer();
	}
	return result;
}

ExecutionEnvironment::ExecutionEnvironment() : m_container_type(get_container_type()) {}

LCC_API_CLOUD_PROVIDER ExecutionEnvironment::guess_cloud_provider_by_os_quirks() const {
	LCC_API_CLOUD_PROVIDER result = PROV_UNKNOWN;
	ifstream cmdline("/proc/cmdline");
	if (cmdline.is_open()) {
		string cmd;
		getline(cmdline, cmd);
		if (cmd.find("azure") != string::npos) {
			result = AZURE_CLOUD;
		} else if (cmd.find("gke") != string::npos || cmd.find("gcp") != string::npos) {
			result = GOOGLE_CLOUD;
		} else if (cmd.find("aws") != string::npos) {
			result = AWS;
		}
	} else {
		ifstream azure("/sys/firmware/acpi/tables/WAET");  // azure
		if (cmdline.is_open()) {
			result = AZURE_CLOUD;
		}
	}

	return result;
}

LCC_API_VIRTUALIZATION_DETAIL ExecutionEnvironment::guess_virtualization_by_os_quirks() const {
	LCC_API_VIRTUALIZATION_DETAIL result = BARE_TO_METAL;
	ifstream dt_compat("/proc/device-tree/compatible");
	if (dt_compat.is_open()) {
		string compat_str;
		getline(dt_compat, compat_str);
		if (compat_str.find("xen") != string::npos) {
			result = V_XEN;
		} else if (compat_str.find("kvm") != string::npos) {
			result = KVM;
		} else if (compat_str.find("qemu") != string::npos) {
			result = QEMU;
		} else if (!compat_str.empty()) {
			result = V_OTHER;
		}
	}
	if (result == BARE_TO_METAL || result == V_OTHER) {
		ifstream qemu_fw("/sys/firmware/qemu_fw_cfg");
		if (qemu_fw.good()) {
			result = QEMU;
			return result;
		}
	}

	// 4. Check kernel start line for hypervisor hints
	ifstream cmdline("/proc/cmdline");
	if (cmdline.is_open()) {
		string cmd;
		getline(cmdline, cmd);
		if (cmd.find("console=hvc") != string::npos || cmd.find("xen_blkfront") != string::npos) {
			result = V_XEN;
		} else if (cmd.find("virtio") != string::npos) {
			result = V_OTHER;
		}
	}
	return result;
}

}  // namespace os
}  // namespace license
