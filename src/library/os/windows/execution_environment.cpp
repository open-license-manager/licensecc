#include <paths.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/utsname.h>

#include "isvm/BIOSReader.h"
#include "isvm/Native.h"
#include "../../base/base.h"
#include "../cpu_info.hpp"
#include "../execution_environment.hpp"

namespace license {
using namespace std;



VIRTUALIZATION ExecutionEnvironment::getVirtualization() {
	VIRTUALIZATION result;
	CpuInfo cpuInfo;
	bool isContainer = false;
	if (isContainer) {
		result = CONTAINER;
	} else if (cpuInfo.cpu_virtual() || is_cloud()) {
		result = VM;
	} else {
		result = NONE;
	}
	return result;
}

bool ExecutionEnvironment::is_cloud() { return getCloudProvider() != ON_PREMISE; }

bool ExecutionEnvironment::is_docker() { return false; }

CLOUD_PROVIDER ExecutionEnvironment::getCloudProvider() {
	if (InitEntryPoints()) {
		BIOSReader reader;
		SystemInformation info = reader.readSystemInfo();

		const char *vmVendors[] = {
			"VMware", "Microsoft Corporation", "Virtual Machine", "innotek GmbH", "PowerVM", "Bochs", "KVM"};

		const int count = _countof(vmVendors);
		for (int i = 0; i != count; ++i) {
			const char *vendor = vmVendors[i];

			if (std::string::npos != info.Manufacturer.find(vendor) ||
				std::string::npos != info.ProductName.find(vendor) ||
				std::string::npos != info.SerialNum.find(vendor)) {
				std::cout << "Inside virual machine!";
				return 1;
			}
		}
	} else {
		return -1;
	}
}

}  // namespace license
