/*
 * virtualization.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */
#include <windows.h>
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

#define MAX_UNITS 20
int wine_container() {
DWORD fileMaxLen;
size_t ndrives = 0;
DWORD fileFlags;
char volName[MAX_PATH], fileSysName[MAX_PATH];
DWORD volSerial = 0;
const DWORD dwSize = MAX_PATH;
char szLogicalDrives[MAX_PATH] = {0};

int result = 0;
const DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

if (dwResult > 0 && dwResult <= MAX_PATH) {
	char* szSingleDrive = szLogicalDrives;
	while (*szSingleDrive && ndrives < MAX_UNITS) {
		// get the next drive
		UINT driveType = GetDriveType(szSingleDrive);
		if (driveType == DRIVE_FIXED) {
			string name = szSingleDrive + string("/var/run/systemd/container");
			try {
				ifstream systemd_container(name);
				if (systemd_container.good()) {
					result = 1;
					for (string line; getline(systemd_container, line);) {
						if (line.find("docker") != string::npos) {
							result = 1;
							break;
						} else if (line.find("lxc") != string::npos) {
							result = 2;
							break;
						}
					}
				}
			} catch (...) {
				//no problem,we're just guessing
			}
		}
	}
}
	return result;
}

bool ExecutionEnvironment::is_docker() const {
	// let's check we're not in linux under wine ;) ...
//int cont = wine_container();
	return false; 
}
//TODO
bool ExecutionEnvironment::is_container() const { return is_docker(); }
}  // namespace os
}  // namespace license
