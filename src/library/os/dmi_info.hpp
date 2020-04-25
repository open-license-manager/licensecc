/*
 * dmi_info.hpp
 *
 *  Created on: Apr 24, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_OS_DMI_INFO_HPP_
#define SRC_LIBRARY_OS_DMI_INFO_HPP_

#include <string>

namespace license {
namespace os {
class DmiInfo {
private:
	std::string m_sys_vendor;
	std::string m_bios_vendor;
	std::string m_bios_description;

public:
	DmiInfo();
	virtual ~DmiInfo(){};
	const std::string& bios_vendor() const { return m_bios_vendor; };
	const std::string& sys_vendor() const { return m_sys_vendor; };
	const std::string& bios_description() const { return m_bios_description; };
};

}  // namespace os
} /* namespace license */

#endif /* SRC_LIBRARY_OS_DMI_INFO_HPP_ */
