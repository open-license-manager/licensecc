#include <Windows.h>
#include <iphlpapi.h>
//definition of size_t
#include <stdlib.h>
#include <stdio.h>
//#include "../../base/base64.h"
#include "../../base/logger.h"
#include"../os.h"
#include "public-key.h"

#pragma comment(lib, "IPHLPAPI.lib")
unsigned char* unbase64(const char* ascii, int len, int *flen);

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]) {
	return FUNC_RET_NOT_AVAIL;
}

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	char buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	BOOL cmpName = GetComputerName(buffer, &bufsize);
	if (cmpName) {
		strncpy(identifier, buffer, 6);
		result = FUNC_RET_OK;
	}
	return result;
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
	return FUNC_RET_NOT_AVAIL;
}

void os_initialize() {
}

//http://www.ok-soft-gmbh.com/ForStackOverflow/EnumMassStorage.c
//http://stackoverflow.com/questions/3098696/same-code-returns-diffrent-result-on-windows7-32-bit-system
#define MAX_UNITS 30
//bug check return with diskinfos == null func_ret_ok
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
	DWORD FileMaxLen;
	int ndrives = 0;
	DWORD FileFlags;
	char volName[_MAX_FNAME], FileSysName[_MAX_FNAME];
	char* szSingleDrive;
	DWORD volSerial = 0;
	BOOL success;
	UINT driveType;
	DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };

	FUNCTION_RETURN return_value;
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH) {
		return_value = FUNC_RET_OK;
		szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && ndrives < MAX_UNITS) {

			// get the next drive
			driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED) {
				success = GetVolumeInformation(szSingleDrive, volName, MAX_PATH,
						&volSerial, &FileMaxLen, &FileFlags, FileSysName,
						MAX_PATH);
				if (success) {
					LOG_INFO("drive         : %s", szSingleDrive);
					LOG_INFO("Volume Name   : %s", volName);
					LOG_INFO("Volume Serial : 0x%x", volSerial); 
					LOG_DEBUG("Max file length : %d", FileMaxLen); 
					LOG_DEBUG("Filesystem      : %s", FileSysName);
					if (diskInfos != NULL) {
						if (ndrives < *disk_info_size) {
							diskInfos[ndrives].id = ndrives;
							strncpy(diskInfos[ndrives].device, volName, MAX_PATH);
							strncpy(diskInfos[ndrives].label, FileSysName, MAX_PATH);
							memcpy(diskInfos[ndrives].disk_sn, &volSerial, sizeof(DWORD));
							diskInfos[ndrives].preferred = (strncmp(szSingleDrive, "C", 1) != 0);
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

static int translate(char ipStringIn[16], unsigned char ipv4[4]) {
	char *str2;

	size_t index = 0;

	str2 = ipStringIn; /* save the pointer */
	while (*str2) {
		if (isdigit((unsigned char) *str2)) {
			ipv4[index] *= 10;
			ipv4[index] += *str2 - '0';
		} else {
			index++;
		}
		str2++;
	}
	return 0;
}

//http://stackoverflow.com/questions/18046063/mac-address-using-c
//TODO: count only interfaces with type (MIB_IF_TYPE_ETHERNET IF_TYPE_IEEE80211)
FUNCTION_RETURN getAdapterInfos(OsAdapterInfo * adapterInfos,
		size_t * adapter_info_size) {
	DWORD dwStatus;
	unsigned int i = 0;
	FUNCTION_RETURN result;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter = NULL;
	//IP_ADAPTER_INFO AdapterInfo[20];              // Allocate information for up to 16 NICs
	DWORD dwBufLen = 0; //10 * sizeof(IP_ADAPTER_INFO);  // Save the memory size of buffer

	i = 3;
	do {
		pAdapterInfo = (PIP_ADAPTER_INFO) malloc(dwBufLen);
		dwStatus = GetAdaptersInfo(               // Call GetAdapterInfo
				pAdapterInfo, // [out] buffer to receive data
				&dwBufLen   // [in] size of receive data buffer
				);
		if (dwStatus != NO_ERROR) {
			free(pAdapterInfo);
		}
	} while (dwStatus == ERROR_BUFFER_OVERFLOW && i-- > 0);

	if (dwStatus == ERROR_BUFFER_OVERFLOW) {
		return FUNC_RET_ERROR;
	}

	if (adapterInfos == NULL || *adapter_info_size == 0) {
		*adapter_info_size = dwBufLen / sizeof(IP_ADAPTER_INFO);
		if (pAdapterInfo != NULL){
			free(pAdapterInfo);
		}
		return FUNC_RET_OK;
	}

	memset(adapterInfos, 0, *adapter_info_size);
	pAdapter = pAdapterInfo;
	i = 0;
	result = FUNC_RET_OK;
	while (pAdapter) {
		strncpy(adapterInfos[i].description, pAdapter->Description,
				min(sizeof(adapterInfos->description),
						MAX_ADAPTER_DESCRIPTION_LENGTH));
		memcpy(adapterInfos[i].mac_address, pAdapter->Address, 8);
		translate(pAdapter->IpAddressList.IpAddress.String,
				adapterInfos[i].ipv4_address);
		adapterInfos[i].type = IFACE_TYPE_ETHERNET;
		i++;
		pAdapter = pAdapter->Next;
		if (i == *adapter_info_size) {
			result = FUNC_RET_BUFFER_TOO_SMALL;
			break;
		}
	}
	free(pAdapterInfo);
	*adapter_info_size = i;
	return result;
}

FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	FUNCTION_RETURN result = FUNC_RET_OK;
	DWORD wres = GetModuleFileName(NULL, buffer, MAX_PATH);
	if (wres == 0) {
		result = FUNC_RET_ERROR;
	}
	return result;
}

static void printHash(HCRYPTHASH* hHash) {
	BYTE *pbHash;
	DWORD dwHashLen;
	DWORD dwHashLenSize = sizeof(DWORD);
	char* hashStr;
	unsigned int i;

	if (CryptGetHashParam(*hHash, HP_HASHSIZE, (BYTE *) &dwHashLen,
			&dwHashLenSize, 0)) {
		pbHash = (BYTE*) malloc(dwHashLen);
		hashStr = (char*) malloc(dwHashLen * 2 + 1);
		if (CryptGetHashParam(*hHash, HP_HASHVAL, pbHash, &dwHashLen, 0)) {
			for (i = 0; i < dwHashLen; i++) {
				sprintf(&hashStr[i * 2], "%02x", pbHash[i]);
			} LOG_DEBUG("Hash to verify: %s", hashStr);
		}
		free(pbHash);
		free(hashStr);
	}
}

FUNCTION_RETURN verifySignature(const char* stringToVerify,
		const char* signatureB64) {
	//--------------------------------------------------------------------
	// Declare variables.
	//
	// hProv:           Cryptographic service provider (CSP). This example
	//                  uses the Microsoft Enhanced Cryptographic 
	//                  Provider.
	// hKey:            Key to be used. In this example, you import the 
	//                  key as a PLAINTEXTKEYBLOB.
	// dwBlobLen:       Length of the plaintext key.
	// pbKeyBlob:       Pointer to the exported key.
	BYTE pubKey[] = PUBLIC_KEY;

	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	HCRYPTHASH hHash = 0;
	DWORD dwSigLen;
	BYTE* sigBlob;

	//--------------------------------------------------------------------
	// Acquire a handle to the CSP.

	if (!CryptAcquireContext(&hProv,
	NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		// If the key container cannot be opened, try creating a new
		// container by specifying a container name and setting the 
		// CRYPT_NEWKEYSET flag.
		LOG_INFO("Error in AcquireContext 0x%08x \n", GetLastError());
		if (NTE_BAD_KEYSET == GetLastError()) {
			if (!CryptAcquireContext(&hProv, "license++verify",
					MS_ENHANCED_PROV, PROV_RSA_FULL,
					CRYPT_NEWKEYSET | CRYPT_VERIFYCONTEXT)) {
				LOG_ERROR("Error in AcquireContext 0x%08x \n", GetLastError());
				return FUNC_RET_ERROR;
			}
		} else {
			LOG_ERROR(" Error in AcquireContext 0x%08x \n", GetLastError());
			return FUNC_RET_ERROR;
		}
	}

	// Use the CryptImportKey function to import the PLAINTEXTKEYBLOB
	// BYTE array into the key container. The function returns a 
	// pointer to an HCRYPTKEY variable that contains the handle of
	// the imported key.
	if (!CryptImportKey(hProv, &pubKey[0], sizeof(pubKey), 0, 0, &hKey)) {
		LOG_ERROR("Error 0x%08x in importing the PublicKey \n", GetLastError());
		return FUNC_RET_ERROR;
	}

	if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
		LOG_DEBUG("Hash object created.");
	} else {
		LOG_ERROR("Error in hash creation 0x%08x ", GetLastError());
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}

	if (!CryptHashData(hHash, stringToVerify, (DWORD) strlen(stringToVerify), 0)) {
		LOG_ERROR("Error in hashing data 0x%08x ", GetLastError());
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}
#ifdef _DEBUG
	LOG_DEBUG("Lenght %d, hashed Data: [%s]", strlen(stringToVerify), stringToVerify);
	printHash(&hHash);
#endif
	sigBlob = unbase64(signatureB64, (int) strlen(signatureB64), &dwSigLen);
	LOG_DEBUG("raw signature lenght %d", dwSigLen);
	if (!CryptVerifySignature(hHash, sigBlob, dwSigLen, hKey, NULL, 0)) {
		LOG_ERROR("Signature not validated!  0x%08x ", GetLastError());
		free(sigBlob);
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return FUNC_RET_ERROR;
	}
	CryptDestroyHash(hHash);
	free(sigBlob);
	CryptReleaseContext(hProv, 0);
	return FUNC_RET_OK;
}

