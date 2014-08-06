#include <Windows.h>
#include"../os.h"

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]){

}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	char* buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	BOOL cmpName = GetComputerName(
		buffer,&bufsize);
	strncpy(identifier, buffer, 6);
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
}

void os_initialize() {
}

#define MAX_UNITS 20
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
}

FUNCTION_RETURN getAdapterInfos(AdapterInfo * adapterInfos,
	size_t * adapter_info_size) {
}