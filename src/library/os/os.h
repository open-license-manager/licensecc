/*
 * os-dependent.hpp
 *
 *  Created on: Mar 29, 2014
 *      Author: devel
 */

#ifndef OS_DEPENDENT_HPP_
#define OS_DEPENDENT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "../base/base.h"
#include <stddef.h>

typedef enum {
	NONE, VMWARE
} VIRTUALIZATION;

typedef struct {
	int id;
	char description[1024];
	unsigned char mac_address[6];
	unsigned char ipv4_address[4];
} AdapterInfo;

typedef struct {
	int id;
	char device[255];
	unsigned char disk_sn[8];
	char label[255];
} DiskInfo;

FUNCTION_RETURN getAdapterInfos(AdapterInfo * adapterInfos,
		size_t * adapter_info_size);
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size);
FUNCTION_RETURN getUserHomePath(char[MAX_PATH]);
VIRTUALIZATION getVirtualization();
void os_initialize();

#ifdef __cplusplus
}
#endif

#endif /* OS_DEPENDENT_HPP_ */
