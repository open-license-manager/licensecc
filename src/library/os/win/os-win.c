#include <Windows.h>
#include <iphlpapi.h>
#include"../os.h"

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]){

}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	char* buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	BOOL cmpName = GetComputerName(
		buffer, &bufsize);
	strncpy(identifier, buffer, 6);
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
}

void os_initialize() {
}

#define MAX_UNITS 20
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
}

//http://stackoverflow.com/questions/18046063/mac-address-using-c
//TODO: count only interfaces with type (MIB_IF_TYPE_ETHERNET IF_TYPE_IEEE80211)
FUNCTION_RETURN getAdapterInfos(OsAdapterInfo * adapterInfos, size_t * adapter_info_size) {
	DWORD dwStatus;
	unsigned int i;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter = NULL;
	//IP_ADAPTER_INFO AdapterInfo[16];              // Allocate information for up to 16 NICs
	DWORD dwBufLen = 0; //sizeof(AdapterInfo);         // Save the memory size of buffer

	dwStatus = GetAdaptersInfo(               // Call GetAdapterInfo
		null,// [out] buffer to receive data
		&dwBufLen   // [in] size of receive data buffer
		);
	if (dwStatus != ERROR_BUFFER_OVERFLOW){
		return FUNC_RET_ERROR;
	}
	if (adapterInfos == null || *adapter_info_size == 0){
		*adapter_info_size = dwBufLen;
		return FUNC_RET_BUFFER_TOO_SMALL;
	}
	else {
		memset(adapterInfos, 0, adapter_info_size);
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen*sizeof(IP_ADAPTER_INFO));
		dwStatus = GetAdaptersInfo(pAdapterInfo, &dwBufLen);
		if (dwStatus != NO_ERROR){
			free(pAdapterInfo);
			return FUNC_RET_ERROR;
		}
		pAdapter = pAdapterInfo;
		for (i = 0; i < min(*adapter_info_size, dwBufLen); i++) {
			strncpy(adapterInfos[i].description, pAdapter->Description, min(sizeof(adapterInfos->description), MAX_ADAPTER_DESCRIPTION_LENGTH));
			memcpy(adapterInfos[i].mac_address, pAdapter->Address, 8);
			memcpy(adapterInfos[i].ipv4_address, pAdapter->IpAddressList.IpAddress, 8);

			pAdapter = pAdapter->Next;
		}
	}

#include <ctype.h>
#include <stdio.h>
	int main(void) {
		char *str = "192.168.0.1", *str2;
		unsigned char value[4] = { 0 };
		size_t index = 0;

		str2 = str; /* save the pointer */
		while (*str) {
			if (isdigit((unsigned char)*str)) {
				value[index] *= 10;
				value[index] += *str - '0';
			}
			else {
				index++;
			}
			str++;
		}
		printf("values in \"%s\": %d %d %d %d\n", str2,
			value[0], value[1], value[2], value[3]);
		return 0;
	}
}