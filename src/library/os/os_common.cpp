#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <cstdint>
#include "../base/string_utils.h"
#include "../base/logger.h"
#include "os_common.h"
#include "os.h"

/**
 * Implementation of DiskInfo::to_string method
 */
std::string DiskInfo::to_string() const {
	std::string result = "DiskInfo{id=" + std::to_string(id) + ", device=" + std::string(device) + ", disk_sn=[";
	for (int i = 0; i < 8; ++i) {
		result += std::to_string(disk_sn[i]);
	}
	result += "], sn_initialized=" + std::to_string(sn_initialized) + ", label=" + std::string(label) +
			  ", label_initialized=" + std::to_string(label_initialized) + ", preferred=" + std::to_string(preferred) +
			  ", physical_serial=" + physical_serial + "}";

	return result;
}

/**
 * Implementation of guidToString
 */
std::string guidToString(const unsigned char (&bytes)[16]) {
	char buffer[40] = {0};
	snprintf(buffer, sizeof(buffer), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			 (unsigned long)((uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) |
							 ((uint32_t)bytes[3] << 24)),
			 (unsigned)((uint16_t)bytes[4] | ((uint16_t)bytes[5] << 8)),
			 (unsigned)((uint16_t)bytes[6] | ((uint16_t)bytes[7] << 8)), bytes[8], bytes[9], bytes[10], bytes[11],
			 bytes[12], bytes[13], bytes[14], bytes[15]);
	return std::string(buffer);
}
