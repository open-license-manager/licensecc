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

#include "../../base/StringUtils.h"
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
	int adapter_info_size;
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);  

	unsigned int i = 3;
	do {
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
		dwStatus = GetAdaptersInfo(	 // Call GetAdapterInfo
			pAdapterInfo,  // [out] buffer to receive data
			&dwBufLen  // [in] size of receive data buffer
		);
		if (dwStatus != NO_ERROR && pAdapterInfo != nullptr) {
			free(pAdapterInfo);
			pAdapterInfo = nullptr;
		}
	} while (dwStatus == ERROR_BUFFER_OVERFLOW && i-- > 0);

	if (dwStatus == ERROR_BUFFER_OVERFLOW) {
		return FUNC_RET_ERROR;
	}

	adapter_info_size = dwBufLen / sizeof(IP_ADAPTER_INFO);
	if (adapter_info_size == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
	i = 0;
	FUNCTION_RETURN result = FUNC_RET_OK;
	while (pAdapter) {
		OsAdapterInfo ai = {};
		strncpy(ai.description, pAdapter->Description,
				min(sizeof(ai.description), MAX_ADAPTER_DESCRIPTION_LENGTH));
		memcpy(ai.mac_address, pAdapter->Address, 8);
		translate(pAdapter->IpAddressList.IpAddress.String, ai.ipv4_address);
		ai.type = IFACE_TYPE_ETHERNET;
		i++;
		pAdapter = pAdapter->Next;
		if (i == adapter_info_size) {
			result = FUNC_RET_BUFFER_TOO_SMALL;
			break;
		}
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
