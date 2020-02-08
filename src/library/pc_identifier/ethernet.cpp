/*
 * ethernet.cpp
 *
 *  Created on: Jan 11, 2020
 *      Author: devel
 */

#include "ethernet.hpp"

#include <bits/stdint-uintn.h>
#include <array>
#include <vector>

#include "../../../include/licensecc/datatypes.h"
#include "../../../projects/DEFAULT/include/licensecc/DEFAULT/licensecc_properties.h"
#include "../base/base.h"
#include "../os/network.hpp"
#include "pc_identifier.hpp"

namespace license {
namespace pc_identifier {
using namespace std;

static FUNCTION_RETURN generate_ethernet_pc_id(vector<array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA>> &data,
											   const bool use_ip) {
	vector<os::OsAdapterInfo> adapters;

	FUNCTION_RETURN result_adapterInfos = getAdapterInfos(adapters);
	if (result_adapterInfos != FUNC_RET_OK) {
		return result_adapterInfos;
	}
	if (adapters.size() == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	for (auto &it : adapters) {
		unsigned int k, data_len, data_byte;
		array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA> identifier;
		data_len = use_ip ? sizeof(os::OsAdapterInfo::ipv4_address) : sizeof(os::OsAdapterInfo::mac_address);

		for (k = 0; k < PC_IDENTIFIER_PROPRIETARY_DATA; k++) {
			if (k < data_len) {
				identifier[k] = use_ip ? it.ipv4_address[k] : it.mac_address[k];
			} else {
				identifier[k] = 42;
			}
		}
		identifier[0] = identifier[0] & 0x1F;
		data.push_back(identifier);
		}

	return result_adapterInfos;
}

Ethernet::Ethernet(bool useIp) : use_ip(useIp) {}

Ethernet::~Ethernet() {}

LCC_API_IDENTIFICATION_STRATEGY Ethernet::identification_strategy() const { return STRATEGY_ETHERNET; }

FUNCTION_RETURN Ethernet::identify_pc(PcIdentifier &pc_id) const {
	vector<array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN result = generate_ethernet_pc_id(data, use_ip);
	if (result == FUNC_RET_OK) {
		pc_id.set_data(data[0]);
	}
	return result;
}

std::vector<PcIdentifier> Ethernet::alternative_ids() const {
	vector<array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN result = generate_ethernet_pc_id(data, use_ip);
	vector<PcIdentifier> identifiers;
	if (result == FUNC_RET_OK) {
		identifiers.resize(data.size());
		for (auto &it : data) {
			PcIdentifier pc_id;
			pc_id.set_identification_strategy(identification_strategy());
			pc_id.set_data(it);
			identifiers.push_back(pc_id);
		}
	}
	return identifiers;
}

LCC_EVENT_TYPE Ethernet::validate_identifier(const PcIdentifier &identifier) const {
	vector<array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN generate_ethernet = generate_ethernet_pc_id(data, use_ip);
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	if (generate_ethernet == FUNC_RET_OK) {
		result = validate_identifier(identifier, data);
	}
	return result;
}

}  // namespace pc_identifier
} /* namespace license */
