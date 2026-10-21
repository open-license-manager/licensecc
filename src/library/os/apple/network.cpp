/**
 * @file network.cpp
 * @brief macOS implementation of network interface detection.
 *
 * Uses getifaddrs(3) with AF_LINK addresses for the MAC, same as the
 * Linux backend. No subprocesses.
 */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <memory.h>
#include <unordered_map>

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../network.hpp"

namespace license {
namespace os {
using namespace std;

FUNCTION_RETURN getAdapterInfos(vector<OsAdapterInfo>& adapterInfos) {
	unordered_map<string, OsAdapterInfo> adapterByName;

	FUNCTION_RETURN f_return = FUNC_RET_OK;
	struct ifaddrs *ifaddr, *ifa;
	int family, n = 0;
	unsigned int if_num;

	if (getifaddrs(&ifaddr) == -1) {
		LOG_WARN("getifaddrs failed == -1");
		return FUNC_RET_ERROR;
	}

	for (ifa = ifaddr, n = 0, if_num = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL || (ifa->ifa_flags & IFF_LOOPBACK) != 0) {
			continue;
		}
		string if_name(ifa->ifa_name, mstrnlen_s(ifa->ifa_name, LCC_ADAPTER_DESCRIPTION_LEN));
		OsAdapterInfo* currentAdapter;
		if (adapterByName.find(if_name) == adapterByName.end()) {
			OsAdapterInfo newAdapter;
			memset(&newAdapter, 0, sizeof(OsAdapterInfo));
			mstrlcpy(&newAdapter.description[0], ifa->ifa_name, LCC_ADAPTER_DESCRIPTION_LEN);
			adapterByName[if_name] = newAdapter;
		}
		auto it = adapterByName.find(if_name);
		currentAdapter = &it->second;
		family = ifa->ifa_addr->sa_family;
		LOG_DEBUG("%-8s %s (%d)\n", ifa->ifa_name,
				  (family == AF_LINK)	 ? "AF_LINK"
				  : (family == AF_INET)	 ? "AF_INET"
				  : (family == AF_INET6) ? "AF_INET6"
										 : "???",
				  family);
		if (family == AF_INET) {
			struct sockaddr_in* s1 = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
			in_addr_t iaddr = s1->sin_addr.s_addr;
			currentAdapter->ipv4_address[0] = (iaddr & 0x000000ff);
			currentAdapter->ipv4_address[1] = (iaddr & 0x0000ff00) >> 8;
			currentAdapter->ipv4_address[2] = (iaddr & 0x00ff0000) >> 16;
			currentAdapter->ipv4_address[3] = (iaddr & 0xff000000) >> 24;
		} else if (family == AF_LINK) {
			struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifa->ifa_addr);
			if (sdl->sdl_alen == 6) {
				const unsigned char* mac = reinterpret_cast<const unsigned char*>(LLADDR(sdl));
				for (int i = 0; i < 6; i++) {
					currentAdapter->mac_address[i] = mac[i];
					LOG_DEBUG("%02x:", mac[i]);
				}
			}
		}
	}
	freeifaddrs(ifaddr);

	if (adapterByName.size() == 0) {
		f_return = FUNC_RET_NOT_AVAIL;
	} else {
		f_return = FUNC_RET_OK;
		adapterInfos.reserve(adapterByName.size());
		for (auto& it : adapterByName) {
			adapterInfos.push_back(it.second);
		}
	}
	return f_return;
}

}  // namespace os
}  // namespace license
