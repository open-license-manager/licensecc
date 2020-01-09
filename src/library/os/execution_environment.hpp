/*
 * virtualization.hpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_OS_VIRTUALIZATION_HPP_
#define SRC_LIBRARY_OS_VIRTUALIZATION_HPP_

namespace license {

typedef enum { NONE, CONTAINER, VM } VIRTUALIZATION;
typedef enum {
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

typedef enum { BARE_TO_METAL, VMWARE, VIRTUALBOX, XEN, KVM } VIRTUALIZATION_DETAIL;

class ExecutionEnvironment {
public:
	ExecutionEnvironment(){};
	virtual ~ExecutionEnvironment(){};
	VIRTUALIZATION getVirtualization();
};

}  // namespace license

#endif /* SRC_LIBRARY_OS_VIRTUALIZATION_HPP_ */
