/*
 * cpu_info.h
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_OS_CPU_INFO_H_
#define SRC_LIBRARY_OS_CPU_INFO_H_
#include <string>
namespace license {

/**
 * Cpu informations
 */
class CpuInfo {
public:
	CpuInfo();
	virtual ~CpuInfo();
	/**
	 * Detect Virtual machine using hypervisor bit or the cpu vendor name.
	 * @return true if the cpu is detected to be a virtual cpu
	 */
	bool cpu_virtual() const;
	uint32_t model();
	std::string vendor() const;
};

} /* namespace license */

#endif /* SRC_LIBRARY_OS_CPU_INFO_H_ */
