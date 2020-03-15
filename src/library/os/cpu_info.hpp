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
namespace os {
typedef enum { BARE_TO_METAL, VMWARE, VIRTUALBOX, V_XEN, KVM, HV, V_OTHER } VIRTUALIZATION_DETAIL;

/**
 * Cpu informations
 */
class CpuInfo {
private:
	bool is_hypervisor_set() const;

public:
	CpuInfo();
	virtual ~CpuInfo();
	/**
	 * Detect Virtual machine using hypervisor bit or the cpu vendor name.
	 * @return true if the cpu is detected to be a virtual cpu
	 */
	bool cpu_virtual() const;
	uint32_t model() const;
	std::string vendor() const;
	// std::string brand() const;
	VIRTUALIZATION_DETAIL getVirtualizationDetail() const;
};

}  // namespace os
}  // namespace license
#endif /* SRC_LIBRARY_OS_CPU_INFO_H_ */
