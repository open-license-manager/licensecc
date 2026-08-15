/*
 * Toghether with execution_environment.hpp they are the interface of the os package.
 *
 *  Created on: Mar 29, 2014
 *
 */

#ifndef OS_DEPENDENT_HPP_
#define OS_DEPENDENT_HPP_

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdlib.h>
#include <vector>
#include <string>
#ifdef __unix__
#include <unistd.h>
#include <stdbool.h>
#endif

#include <licensecc/datatypes.h>
#include "../base/base.h"

typedef struct {
	int id;
	char device[MAX_PATH];
	unsigned char disk_sn[8];
	bool sn_initialized;
	char label[255];
	bool label_initialized = false;
	bool preferred = false;

	/**
	 * Before 2.1.0
	 * BUG: physical serial is only available in Linux if root.
	 * remove this. improve the disk_sn (that is already doing the good stuff)
	 * (just need to remove the -partxx in the end if present) and change disk_sn to string.
	 * For windows use the same disk_sn both for physical and logical volume(as fallback.)
	 */

	std::string physical_serial;
	bool physical_serial_initialized = false;

	std::string to_string() const;
} DiskInfo;

FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo>& diskInfos);
FUNCTION_RETURN getUserHomePath(char[MAX_PATH]);
FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]);
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
FUNCTION_RETURN getOsSpecificIdentifier(std::string& identifier);

#ifdef _WIN32
#define SETENV(VAR, VAL) _putenv_s(VAR, VAL);
#define UNSETENV(P) _putenv_s(P, "");
#else
#define SETENV(VAR, VAL) setenv(VAR, VAL, 1);
#define UNSETENV(P) unsetenv(P);
#endif

#endif /* OS_DEPENDENT_HPP_ */
