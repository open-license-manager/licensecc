/*
 * network.hpp
 *
 *  Created on: Feb 8, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_OS_NETWORK_HPP_
#define SRC_LIBRARY_OS_NETWORK_HPP_
#include <stdlib.h>
#include <vector>

#include "../base/base.h"

namespace license {
namespace os {

typedef enum { IFACE_TYPE_ETHERNET, IFACE_TYPE_WIRELESS } IFACE_TYPE;

typedef struct {
	int id;
	char description[1024];
	unsigned char mac_address[8];
	unsigned char ipv4_address[4];
	IFACE_TYPE type;
} OsAdapterInfo;

FUNCTION_RETURN getAdapterInfos(std::vector<OsAdapterInfo>& adapterInfos);

}  // namespace os
}  // namespace license
#endif /* SRC_LIBRARY_OS_NETWORK_HPP_ */
