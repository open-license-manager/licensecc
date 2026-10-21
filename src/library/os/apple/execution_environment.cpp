/*
 * execution_environment.cpp - macOS implementation
 *
 * Containers (Docker, LXC) run their workloads inside a Linux virtual
 * machine on macOS, so a native macOS process is never containerized.
 * VM detection uses board/CPU identifiers reported by the major
 * virtualization products (see also cpu_info.cpp / board_info.cpp).
 */

#include <string>
#include <sys/sysctl.h>

#include "../../base/base.h"
#include "../cpu_info.hpp"
#include "../execution_environment.hpp"
#include "../../base/string_utils.h"

namespace license {
namespace os {
using namespace std;

/**
 * Read a sysctl string parameter, e.g. "hw.model".
 */
static string get_sysctl_string(const char* name) {
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

static CONTAINER_TYPE get_container_type() {
	// containers on macOS run in a Linux VM, a native process is never containerized
	return CONTAINER_TYPE::NONE;
}

ExecutionEnvironment::ExecutionEnvironment() : m_container_type(get_container_type()) {}

LCC_API_CLOUD_PROVIDER ExecutionEnvironment::guess_cloud_provider_by_os_quirks() const {
	// no known cloud provider quirks for macOS hosts (EC2 Mac etc. are bare metal)
	return PROV_UNKNOWN;
}

LCC_API_VIRTUALIZATION_DETAIL ExecutionEnvironment::guess_virtualization_by_os_quirks() const {
	// VM products put their name in hw.model, bare metal reports a Mac model id
	const string hw_model = toupper_copy(trim_copy(get_sysctl_string("hw.model")));
	if (hw_model.find("PARALLELS") != string::npos) {
		return PARALLELS;
	} else if (hw_model.find("VMWARE") != string::npos) {
		return VMWARE;
	} else if (hw_model.find("VIRTUALBOX") != string::npos) {
		return VIRTUALBOX;
	} else if (hw_model.find("QEMU") != string::npos) {
		return QEMU;
	} else if (hw_model.find("VIRTUAL") != string::npos) {
		// Apple Virtualization framework guests (e.g. UTM, Tart)
		return V_OTHER;
	}
	return BARE_TO_METAL;
}

}  // namespace os
}  // namespace license
