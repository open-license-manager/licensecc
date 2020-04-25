/*
 * dmi_info.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: devel
 */

#include <windows.h>
#include "isvm/BIOSReader.h"
#include "isvm/Native.h"
#include "../../base/StringUtils.h"
#include "../dmi_info.hpp"

namespace license {
namespace os {
DmiInfo::DmiInfo() {
	if (InitEntryPoints()) {
		BIOSReader reader;
		SystemInformation info = reader.readSystemInfo();
		m_sys_vendor = toupper_copy(info.Manufacturer);
		m_bios_vendor = toupper_copy(info.ProductName);
		m_bios_description = toupper_copy(info.SysVersion) + toupper_copy(info.family);
	}
}
}
} /* namespace license */
