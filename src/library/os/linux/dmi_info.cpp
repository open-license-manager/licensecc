
#include "../../base/file_utils.hpp"
#include "../../base/StringUtils.h"

#include "../dmi_info.hpp"

namespace license {
namespace os {

DmiInfo::DmiInfo() {
	try {
		m_bios_vendor = toupper_copy(trim_copy(get_file_contents("/sys/class/dmi/id/sys_vendor", 256)));

	} catch (...) {
	}
	try {
		m_bios_description = toupper_copy(trim_copy(get_file_contents("/sys/class/dmi/id/modalias", 256)));
		char last_char = m_bios_description[m_bios_description.length() - 1];
		if (last_char == '\r' || last_char == '\n') {
			m_bios_description = m_bios_description.erase(m_bios_description.length() - 1);
		}
	} catch (...) {
	}
	try {
		m_sys_vendor = get_file_contents("/sys/class/dmi/id/sys_vendor", 256);
		char last_char = m_sys_vendor[m_sys_vendor.length() - 2];
		if (last_char == '\r' || last_char == '\n') {
			m_sys_vendor = m_sys_vendor.erase(m_sys_vendor.length() - 1);
		}
	} catch (...) {
	}
}

}  // namespace os
}  // namespace license
