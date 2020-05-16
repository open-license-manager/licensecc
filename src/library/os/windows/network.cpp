/**
 * @file network_id.c
 * @date 16 Sep 2014
 * @brief File containing network interface detection functions for Windows.
 *
 * The only public function of this module is #getAdapterInfos(OsAdapterInfo *,
 *		size_t *), other functions are either static or inline.
 *
 * Responsibility of this module is to fill OsAdapterInfo structures, in a predictable way (skip loopback/vpn interfaces)
 */

#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <iphlpapi.h>
#include <unordered_map>
#include <stdio.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../network.hpp"

namespace license {
namespace os {
using namespace std;

static int translate(char ipStringIn[16], unsigned char ipv4[4]) {
	size_t index = 0;

	char *str2 = ipStringIn; /* save the pointer */
	while (*str2) {
		if (isdigit((unsigned char)*str2)) {
			ipv4[index] *= 10;
			ipv4[index] += *str2 - '0';
		} else {
			index++;
		}
		str2++;
	}
	return 0;
}
/**
 *
 * @param adapterInfos
 * @param adapter_info_size
 * @return
 */
FUNCTION_RETURN getAdapterInfos(vector<OsAdapterInfo> &adapterInfos) {
	unordered_map<string, OsAdapterInfo> adapterByName;
	FUNCTION_RETURN f_return = FUNC_RET_OK;
	DWORD dwStatus;
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
	if (pAdapterInfo == nullptr) {
		return FUNC_RET_ERROR;
	}

	dwStatus = GetAdaptersInfo(	 // Call GetAdapterInfo
		pAdapterInfo,  // [out] buffer to receive data
		&dwBufLen  // [in] size of receive data buffer
	);

	// Incase the buffer was too small, reallocate with the returned dwBufLen
	if (dwStatus == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);

		// Will only fail if buffer cannot be allocated (out of memory)
		if (pAdapterInfo == nullptr) {
			return FUNC_RET_BUFFER_TOO_SMALL;
		}

		dwStatus = GetAdaptersInfo(	 // Call GetAdapterInfo
			pAdapterInfo,  // [out] buffer to receive data
			&dwBufLen  // [in] size of receive data buffer
		);

		switch (dwStatus) {
			case NO_ERROR:
				break;

			case ERROR_BUFFER_OVERFLOW:
				free(pAdapterInfo);
				return FUNC_RET_BUFFER_TOO_SMALL;

			default:
				free(pAdapterInfo);
				return FUNC_RET_ERROR;
		}
	}

	PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
	FUNCTION_RETURN result = FUNC_RET_OK;
	while (pAdapter) {
		OsAdapterInfo ai = {};
		strncpy(ai.description, pAdapter->Description,
				min((int)sizeof(ai.description), MAX_ADAPTER_DESCRIPTION_LENGTH));
		memcpy(ai.mac_address, pAdapter->Address, 8);
		translate(pAdapter->IpAddressList.IpAddress.String, ai.ipv4_address);
		ai.type = IFACE_TYPE_ETHERNET;
		pAdapter = pAdapter->Next;
		adapterByName[string(ai.description)] = ai;
	}
	free(pAdapterInfo);

	// FIXME sort by eth , enps, wlan
	if (adapterByName.size() == 0) {
		f_return = FUNC_RET_NOT_AVAIL;
	} else {
		f_return = FUNC_RET_OK;
		adapterInfos.reserve(adapterByName.size());
		for (auto &it : adapterByName) {
			adapterInfos.push_back(it.second);
		}
	}
	return f_return;
}

}  // namespace os
}  // namespace license
