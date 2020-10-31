/*
 * dmi_info.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: devel
 */

#include "isvm/BIOSReader.h"
#include "../../base/string_utils.h"
#include "../dmi_info.hpp"

namespace license {
namespace os {
DmiInfo::DmiInfo() {
	
		BIOSReader reader;
		SystemInformation info = reader.readSystemInfo();
		m_sys_vendor = toupper_copy(info.Manufacturer);
		m_bios_vendor = toupper_copy(info.ProductName);
		m_bios_description = toupper_copy(info.SysVersion) + toupper_copy(info.family);
}
}
} /* namespace license */
