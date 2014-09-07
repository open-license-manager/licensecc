#include <Windows.h>
#include <iphlpapi.h>
#include"../os.h"
#pragma comment(lib, "IPHLPAPI.lib")

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]){

}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	char buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	BOOL cmpName = GetComputerName(
		buffer, &bufsize);
	strncpy(identifier, buffer, 6);
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
}

void os_initialize() {
}

//http://www.ok-soft-gmbh.com/ForStackOverflow/EnumMassStorage.c
//http://stackoverflow.com/questions/3098696/same-code-returns-diffrent-result-on-windows7-32-bit-system
#define MAX_UNITS 30
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
	DWORD FileMaxLen;
	int ndrives = 0;
	DWORD FileFlags;
	char volName[_MAX_FNAME], FileSysName[_MAX_FNAME];
	DWORD volSerial = 0;
	BOOL success;
	UINT driveType;
	DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };

	FUNCTION_RETURN return_value;
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		return_value = FUNC_RET_OK;
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && ndrives < MAX_UNITS)
		{

			// get the next drive
			driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED){
				success = GetVolumeInformation(szSingleDrive, volName, MAX_PATH, &volSerial,
					&FileMaxLen, &FileFlags, FileSysName, MAX_PATH);
				if (success) {
#ifdef _DEBUG
					printf("drive         : %s\n", szSingleDrive);
					printf("Volume Name   : %s\n", volName);
					printf("Volume Serial : 0x%x\n", volSerial);
					printf("Max file length : %d\n", FileMaxLen);
					printf("Filesystem      : %s\n", FileSysName);
#endif
					if (diskInfos != NULL && * disk_info_size<ndrives){
						strncpy(diskInfos[ndrives].device,volName,MAX_PATH);
						strncpy(diskInfos[ndrives].label, FileSysName , MAX_PATH);
						diskInfos[ndrives].id=ndrives;
						diskInfos[ndrives].preferred = (strncmp(szSingleDrive,"C",1)!=0);

					}
					ndrives++;
				}
				else {
					printf("Unable to retreive information of '%s'\n", szSingleDrive);
				}
			}
#ifdef _DEBUG
			printf("This volume is not a fixed disk : %s\n", szSingleDrive);
#endif
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}

	if (*disk_info_size >= ndrives){
		return_value = FUNC_RET_BUFFER_TOO_SMALL;
	}
	return return_value;
}

static int translate(char ipStringIn[16], unsigned char ipv4[4]) {
	char *str2;

	size_t index = 0;

	str2 = ipStringIn; /* save the pointer */
	while (*str2) {
		if (isdigit((unsigned char)*str2)) {
			ipv4[index] *= 10;
			ipv4[index] += *str2 - '0';
		}
		else {
			index++;
		}
		str2++;
	}
	return 0;
}

//http://stackoverflow.com/questions/18046063/mac-address-using-c
//TODO: count only interfaces with type (MIB_IF_TYPE_ETHERNET IF_TYPE_IEEE80211)
FUNCTION_RETURN getAdapterInfos(OsAdapterInfo * adapterInfos, size_t * adapter_info_size) {
	DWORD dwStatus;
	unsigned int i = 0;
	FUNCTION_RETURN result;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter = NULL;
	//IP_ADAPTER_INFO AdapterInfo[20];              // Allocate information for up to 16 NICs
	DWORD dwBufLen = 20; //sizeof(AdapterInfo);         // Save the memory size of buffer

	i = 3;
	do{
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO)*dwBufLen);
		dwStatus = GetAdaptersInfo(               // Call GetAdapterInfo
			pAdapterInfo, // [out] buffer to receive data
			&dwBufLen   // [in] size of receive data buffer
			);
		dwBufLen = dwBufLen / sizeof(IP_ADAPTER_INFO);
		if (dwStatus != NO_ERROR){
			free(pAdapterInfo);
		}
	} while (dwStatus == ERROR_BUFFER_OVERFLOW && i-- > 0);

	if (dwStatus != ERROR_BUFFER_OVERFLOW){
		return FUNC_RET_ERROR;
	}
	if (adapterInfos == NULL || *adapter_info_size == 0){
		*adapter_info_size = dwBufLen;
		free(pAdapterInfo);
		return FUNC_RET_BUFFER_TOO_SMALL;
	}

	memset(adapterInfos, 0,*adapter_info_size);
	pAdapter = pAdapterInfo;
	i = 0;
	result = FUNC_RET_OK;
	while (pAdapter) {
		strncpy(adapterInfos[i].description, pAdapter->Description, min(sizeof(adapterInfos->description), MAX_ADAPTER_DESCRIPTION_LENGTH));
		memcpy(adapterInfos[i].mac_address, pAdapter->Address, 8);
		translate(pAdapter->IpAddressList.IpAddress.String, adapterInfos[i].ipv4_address);
		i++;
		pAdapter = pAdapter->Next;
		if (i == *adapter_info_size){
			result = FUNC_RET_BUFFER_TOO_SMALL;
			break;
		}
	}
	free(pAdapterInfo);
	*adapter_info_size = i;
	return result;
}

