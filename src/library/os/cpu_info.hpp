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
	const int m_cpu_cores;
	const int m_max_cpu;

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
	/**
	 * Number of logical processors of the system.
	 * On Linux it is the total number of configured processors
	 * (sysconf _SC_NPROCESSORS_CONF), ignoring any CPU affinity or cgroup limit.
	 * On Windows it is the number of processors reported by GetSystemInfo.
	 * @return number of logical processors, 0 if it cannot be determined
	 */
	int get_cpu_cores() const { return m_cpu_cores; }
	/**
	 * Maximum number of logical processors usable by the current process.
	 * On Linux it is the number of online processors available to the process
	 * (sysconf _SC_NPROCESSORS_ONLN), honoring CPU affinity and cgroup limits.
	 * On Windows it is the same value as get_cpu_cores().
	 * @return number of logical processors, 0 if it cannot be determined
	 */
	int get_max_cpu() const { return m_max_cpu; }
};

}  // namespace os
}  // namespace license
#endif /* SRC_LIBRARY_OS_CPU_INFO_H_ */
