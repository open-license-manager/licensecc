#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <licensecc/datatypes.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "../../base/logger.h"
#include "../os.h"
using namespace std;

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	char buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	const BOOL cmpName = GetComputerName(buffer, (unsigned long*)&bufsize);
	if (cmpName) {
		strncpy((char*)identifier, buffer, 6);
		result = FUNC_RET_OK;
	}
	return result;
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
	return FUNC_RET_NOT_AVAIL;
}

//http://www.ok-soft-gmbh.com/ForStackOverflow/EnumMassStorage.c
//http://stackoverflow.com/questions/3098696/same-code-returns-diffrent-result-on-windows7-32-bit-system
#define MAX_UNITS 30
//bug check return with diskinfos == null func_ret_ok
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
	DWORD fileMaxLen;
	size_t ndrives = 0;
	DWORD fileFlags;
	char volName[MAX_PATH], fileSysName[MAX_PATH];
	DWORD volSerial = 0;
	const DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };


	FUNCTION_RETURN return_value = FUNC_RET_NOT_AVAIL;
	const DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH) {
		return_value = FUNC_RET_OK;
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && ndrives < MAX_UNITS) {

			// get the next drive
			UINT driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED) {
				BOOL success = GetVolumeInformation(szSingleDrive, volName, MAX_PATH,
				                                    &volSerial, &fileMaxLen, &fileFlags, fileSysName,
				                                    MAX_PATH);
				if (success) {
					LOG_INFO("drive         : %s", szSingleDrive);
					LOG_INFO("Volume Name   : %s", volName);
					LOG_INFO("Volume Serial : 0x%x", volSerial);
					LOG_DEBUG("Max file length : %d", fileMaxLen);
					LOG_DEBUG("Filesystem      : %s", fileSysName);
					if (diskInfos != NULL) {
						if (ndrives < (int)*disk_info_size) {
							diskInfos[ndrives].id = (int)ndrives;
							strncpy(diskInfos[ndrives].device, volName,
									min(std::size_t{MAX_PATH}, sizeof(volName)) - 1);
							strncpy(diskInfos[ndrives].label, fileSysName,
									min(sizeof(diskInfos[ndrives].label), sizeof(fileSysName)) - 1);
							memcpy(diskInfos[ndrives].disk_sn, &volSerial, sizeof(DWORD));
							diskInfos[ndrives].preferred = (szSingleDrive[0] == 'C');
						} else {
							return_value = FUNC_RET_BUFFER_TOO_SMALL;
						}
					}
					ndrives++;
				} else {
					LOG_WARN("Unable to retrieve information of '%s'", szSingleDrive);
				}
			} else {
				LOG_INFO("This volume is not fixed : %s, type: %d",	szSingleDrive);
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}
	if (diskInfos == NULL || *disk_info_size == 0) {
		if (ndrives > 0) {
			return_value = FUNC_RET_OK;
		} else {
			return_value = FUNC_RET_NOT_AVAIL;
			LOG_INFO("No fixed drive was detected");
		}
		*disk_info_size = ndrives;
	} else {
		*disk_info_size = min(ndrives, *disk_info_size);
	}
	return return_value;
}


FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	FUNCTION_RETURN result = FUNC_RET_OK;
	const DWORD wres = GetModuleFileName(NULL, buffer, MAX_PATH);
	if (wres == 0) {
		result = FUNC_RET_ERROR;
	}
	return result;
}

