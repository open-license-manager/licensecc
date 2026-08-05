/*
 * cpu_info.h
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_OS_CPU_INFO_H_
#define SRC_LIBRARY_OS_CPU_INFO_H_
#include <cstdint>
#include <string>
namespace license {
namespace os {

/**
 * Cpu informations
 */
class CpuInfo {
private:
	const std::string m_vendor;
	const std::string m_brand;

public:
	CpuInfo();
	virtual ~CpuInfo();
	uint32_t model() const;
	/**
	 * Detect Virtual machine using hypervisor bit or the cpu vendor name.
	 * @return true if the cpu is detected to be a virtual cpu
	 */
	bool is_virtual() const;
	/**
	 * Check if virtualization information is available.
	 * @return true for Windows and Linux Intel processors, false for ARM
	 */
	bool virt_info_available() const;
	const std::string& vendor() const { return m_vendor; }
	const std::string& brand() const { return m_brand; }
};

}  // namespace os
}  // namespace license
#endif /* SRC_LIBRARY_OS_CPU_INFO_H_ */
