/*
 * virtualization.hpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_OS_VIRTUALIZATION_HPP_
#define SRC_LIBRARY_OS_VIRTUALIZATION_HPP_

#include <string>

namespace license {
namespace os {

/*
 * windows bios sometimes reports vm names add execution environment detection from bios
const char *vmVendors[] = {
	"VMware", "Microsoft Corporation", "Virtual Machine", "innotek GmbH", "PowerVM", "Bochs", "KVM"};
*/

typedef enum { NONE, CONTAINER, VM } VIRTUALIZATION;

typedef enum {
	PROV_UNKNOWN,
	ON_PREMISE,
	GOOGLE_CLOUD,
	AZURE_CLOUD,
	AWS,
	/**
	 * "/sys/class/dmi/id/bios_vendor" SeaBIOS
	 * "/sys/class/dmi/id/sys_vendor" Alibaba Cloud
	 * modalias
	 * "dmi:bvnSeaBIOS:bvrrel-1.7.5-0-ge51488c-20140602_164612-nilsson.home.kraxel.org:bd04/01/2014:svnAlibabaCloud:pnAlibabaCloudECS:pvrpc-i440fx-2.1:cvnAlibabaCloud:ct1:cvrpc-i440fx-2.1:"
	 */
	ALI_CLOUD
} CLOUD_PROVIDER;

class ExecutionEnvironment {
private:
	std::string sys_vendor;
	std::string bios_vendor;
	std::string bios_description;
	// detect if it's a kind of container technology (docker or lxc)
	bool is_container() const;

public:
	ExecutionEnvironment();
	~ExecutionEnvironment(){};
	VIRTUALIZATION getVirtualization() const;
	bool is_cloud() const;
	bool is_docker() const;
	CLOUD_PROVIDER getCloudProvider() const;
};

}  // namespace os
}  // namespace license

#endif /* SRC_LIBRARY_OS_VIRTUALIZATION_HPP_ */
