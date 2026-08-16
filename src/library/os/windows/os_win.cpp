#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <cwctype>
#include <licensecc/datatypes.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../os.h"
#include "../os_common.h"
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

// http://www.ok-soft-gmbh.com/ForStackOverflow/EnumMassStorage.c
// http://stackoverflow.com/questions/3098696/same-code-returns-diffrent-result-on-windows7-32-bit-system
#define MAX_UNITS 40
// bug check return with diskinfos == null func_ret_ok
FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo>& diskInfos) {
	DWORD fileMaxLen;
	size_t ndrives = 0, drives_scanned = 0;
	DWORD fileFlags;
	char volName[MAX_PATH];
	DWORD volSerial = 0;
	const DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = {0};

	FUNCTION_RETURN return_value;
	const DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0) {
		return_value = FUNC_RET_OK;
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && drives_scanned < MAX_UNITS) {
			// get the next drive
			UINT driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED) {
				char fileSysName[MAX_PATH];
				BOOL success = GetVolumeInformation(szSingleDrive, volName, MAX_PATH, &volSerial, &fileMaxLen,
													&fileFlags, fileSysName, MAX_PATH);
				if (success) {
					LOG_DEBUG("drive: %s,volume Name: %s, Volume Serial: 0x%x,Filesystem: %s", szSingleDrive, volName,
							  volSerial, fileSysName);
					DiskInfo diskInfo = {};
					diskInfo.id = (int)ndrives;
					diskInfo.label_initialized = true;
					license::mstrlcpy(diskInfo.device, volName, min(std::size_t{MAX_PATH}, sizeof(volName)));
					license::mstrlcpy(diskInfo.label, fileSysName,
									  min(sizeof(diskInfos[ndrives].label), sizeof(fileSysName)));
					diskInfo.disk_sn = std::to_string(volSerial);
					diskInfo.sn_initialized = true;
					diskInfo.preferred = (szSingleDrive[0] == 'C');
					diskInfos.push_back(diskInfo);
					ndrives++;
				} else {
					LOG_DEBUG("Unable to retrieve information of '%s'", szSingleDrive);
				}
			} else {
				LOG_DEBUG("This volume is not fixed : %s, type: %d", szSingleDrive);
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
			drives_scanned++;
		}
	}
	if (diskInfos.size() > 0) {
		return_value = FUNC_RET_OK;
	} else {
		return_value = FUNC_RET_NOT_AVAIL;
		LOG_DEBUG("No fixed drive were detected");
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

/*
 * GetSystemIdForPublisher is an *undocumented* API (not declared in any public
 * Windows SDK header). It is exported from kernel32.dll (forwarded to
 * api-ms-win-core-sysinfo-l1-2-3.dll) only on Windows 10 1703 (Creators Update)
 * and later, and returns a 16-byte GUID-like identifier tied to the Windows
 * installation.
 *
 * We declare the prototype ourselves and resolve it at runtime with
 * GetProcAddress, so that on older Windows 10 builds (1507/1511/1607), where the
 * export does not exist, the module still loads and we can fall back to the
 * registry instead of failing at load time. Requires no special privileges.
 */
typedef BOOL(WINAPI* PFN_GET_SYSTEM_ID_FOR_PUBLISHER)(BYTE* buffer, DWORD* buffer_size);

static FUNCTION_RETURN getSystemIdForPublisher(std::string& identifier) {
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	if (hKernel32 == NULL) {
		return FUNC_RET_NOT_AVAIL;
	}
	PFN_GET_SYSTEM_ID_FOR_PUBLISHER pfn =
		reinterpret_cast<PFN_GET_SYSTEM_ID_FOR_PUBLISHER>(GetProcAddress(hKernel32, "GetSystemIdForPublisher"));
	if (pfn == NULL) {
		// Not available before Windows 10 1703.
		return FUNC_RET_NOT_AVAIL;
	}
	BYTE systemId[16] = {0};
	DWORD systemIdSize = sizeof(systemId);
	if (!pfn(systemId, &systemIdSize)) {
		LOG_DEBUG("GetSystemIdForPublisher failed, error: %lu", (unsigned long)GetLastError());
		return FUNC_RET_NOT_AVAIL;
	}
	identifier = guidToString(systemId);
	return FUNC_RET_OK;
}

static std::string wideStringToString(const std::wstring& wstr) {
	if (wstr.empty()) {
		return std::string();
	}
	const int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
	if (size <= 0) {
		return std::string();
	}
	std::string str(static_cast<size_t>(size), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size, NULL, NULL);
	return str;
}

static FUNCTION_RETURN readRegistryString(HKEY rootKey, const wchar_t* subKey, const wchar_t* valueName,
										  std::string& out) {
	HKEY hKey = NULL;
	LONG result = RegOpenKeyExW(rootKey, subKey, 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS) {
		LOG_DEBUG("RegOpenKeyExW(%ls) failed, error: %ld", subKey, result);
		return FUNC_RET_NOT_AVAIL;
	}
	DWORD type = 0;
	DWORD dataSize = 0;
	result = RegQueryValueExW(hKey, valueName, NULL, &type, NULL, &dataSize);
	if (result != ERROR_SUCCESS || type != REG_SZ || dataSize == 0) {
		RegCloseKey(hKey);
		return FUNC_RET_NOT_AVAIL;
	}
	std::wstring value(dataSize / sizeof(wchar_t), L'\0');
	result = RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &dataSize);
	RegCloseKey(hKey);
	if (result != ERROR_SUCCESS) {
		return FUNC_RET_NOT_AVAIL;
	}
	// Strip anything after the terminating NUL and any trailing whitespace.
	std::wstring::size_type end = value.find(L'\0');
	std::wstring trimmed = value.substr(0, end);
	while (!trimmed.empty() && iswspace(trimmed[trimmed.size() - 1])) {
		trimmed.erase(trimmed.size() - 1);
	}
	if (trimmed.empty()) {
		return FUNC_RET_NOT_AVAIL;
	}
	out = wideStringToString(trimmed);
	return out.empty() ? FUNC_RET_NOT_AVAIL : FUNC_RET_OK;
}

FUNCTION_RETURN getOsSpecificIdentifier(std::string& identifier) {
	// 1) Preferred: undocumented GetSystemIdForPublisher (Windows 10 1703+),
	//    resolved at runtime so older Windows 10 builds fall through cleanly.
	FUNCTION_RETURN ret = getSystemIdForPublisher(identifier);
	if (ret == FUNC_RET_OK) {
		return ret;
	}
	ret = readRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", identifier);
	if (ret == FUNC_RET_OK) {
		return ret;
	}
	ret = readRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductId",
							 identifier);
	return ret;
}
