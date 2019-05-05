/*
 * os-dependent.hpp
 *
 *  Created on: Mar 29, 2014
 *      
 */

#ifndef OS_DEPENDENT_HPP_
#define OS_DEPENDENT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "../base/base.h"
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
//definition of size_t
#include <stdlib.h>
#ifdef __unix__
#include <unistd.h>
#include <stdbool.h>
#endif

typedef enum {
	NONE, VMWARE
} VIRTUALIZATION;

typedef enum {
	IFACE_TYPE_ETHERNET, IFACE_TYPE_WIRELESS
} IFACE_TYPE;

typedef struct {
	int id;
	char description[1024];
	unsigned char mac_address[8];
	unsigned char ipv4_address[4];
	IFACE_TYPE type;
} OsAdapterInfo;

typedef struct {
	int id;
	char device[255];
	unsigned char disk_sn[8];
	char label[255];
	bool preferred;
} DiskInfo;

FUNCTION_RETURN getAdapterInfos(OsAdapterInfo * adapterInfos,
		size_t * adapter_info_size);
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size);
FUNCTION_RETURN getUserHomePath(char[MAX_PATH]);
FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]);
FUNCTION_RETURN getCpuId(unsigned char identifier[6]);
FUNCTION_RETURN getMachineName(unsigned char identifier[6]);
/**
 * Get an identifier of the machine in an os specific way.
 * In Linux it uses:
 * http://stackoverflow.com/questions/328936/getting-a-unique-id-from-a-unix-like-system
 *
 * <ul>
 * <li>Dbus if available</li>
 * </ul>
 * Can be used as a fallback in case no other methods are available.
 * Windows:
 * HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\ CurrentVersion\ProductId
 * http://sowkot.blogspot.it/2008/08/generating-unique-keyfinger-print-for.html
 * http://stackoverflow.com/questions/2842116/reliable-way-of-generating-unique-hardware-id
 *
 *
 * @param identifier
 * @return
 */
FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]);
VIRTUALIZATION getVirtualization();
void os_initialize();

FUNCTION_RETURN verifySignature(const char* stringToVerify, const char* signatureB64);

#ifdef __cplusplus
}
#endif

#endif /* OS_DEPENDENT_HPP_ */
