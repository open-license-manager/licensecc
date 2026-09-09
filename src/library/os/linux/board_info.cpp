
#include "../../base/file_utils.hpp"
#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../board_info.hpp"

namespace license {
namespace os {

BoardInfo::BoardInfo() {
	std::string file_content;
	if (get_file_contents("/sys/class/dmi/id/bios_vendor", 256, file_content) == FUNC_RET_OK) {
		m_bios_vendor = toupper_copy(trim_copy(file_content));
	} else {
		m_bios_vendor = "";
		LOG_DEBUG("Can not read bios_vendor");
	}
	if (get_file_contents("/sys/class/dmi/id/modalias", 256, file_content) == FUNC_RET_OK) {
		m_bios_description = toupper_copy(trim_copy(file_content));
		if (!m_bios_description.empty()) {
			const char last_char = m_bios_description[m_bios_description.length() - 1];
			if (last_char == '\r' || last_char == '\n') {
				m_bios_description.erase(m_bios_description.length() - 1);
			}
		}
	} else {
		m_bios_description = "";
		LOG_DEBUG("Can not read bios_description");
	}
	if (get_file_contents("/sys/class/dmi/id/sys_vendor", 256, file_content) == FUNC_RET_OK) {
		m_sys_vendor = toupper_copy(trim_copy(file_content));
		if (!m_sys_vendor.empty()) {
			const char last_char = m_sys_vendor[m_sys_vendor.length() - 1];
			if (last_char == '\r' || last_char == '\n') {
				m_sys_vendor.erase(m_sys_vendor.length() - 1);
			}
		}
	} else {
		m_sys_vendor = "";
		LOG_DEBUG("Can not read sys_vendor");
	}
}

}  // namespace os
}  // namespace license
