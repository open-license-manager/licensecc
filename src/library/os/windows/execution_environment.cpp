/*
 * virtualization.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "isvm/BIOSReader.h"
#include "isvm/Native.h"
#include "../../base/base.h"
#include "../../base/StringUtils.h"
#include "../cpu_info.hpp"
#include "../execution_environment.hpp"

namespace license {
namespace os {
using namespace std;

ExecutionEnvironment::ExecutionEnvironment() {
	if (InitEntryPoints()) {
		BIOSReader reader;
		SystemInformation info = reader.readSystemInfo();
		sys_vendor = toupper_copy(info.Manufacturer);
		bios_vendor = toupper_copy(info.ProductName);
		bios_description = toupper_copy(info.SysVersion) + toupper_copy(info.family);
	}
}

//TODO
bool ExecutionEnvironment::is_docker() const { return false; }
//TODO
bool ExecutionEnvironment::is_container() const { return is_docker(); }
}  // namespace os
}  // namespace license
