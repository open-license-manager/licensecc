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
		m_sys_vendor = toupper_copy(info.Manufacturer);
		m_bios_vendor = toupper_copy(info.ProductName);
		m_bios_description = toupper_copy(info.SysVersion) + toupper_copy(info.family);
	}
}

//TODO
bool ExecutionEnvironment::is_docker() const { return false; }
//TODO
bool ExecutionEnvironment::is_container() const { return is_docker(); }
}  // namespace os
}  // namespace license
