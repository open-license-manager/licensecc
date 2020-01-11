/*
 * ethernet.cpp
 *
 *  Created on: Jan 11, 2020
 *      Author: devel
 */
#include <array>

#include "ethernet.hpp"
#include "../os/os.h"

namespace license {
using namespace std;

static FUNCTION_RETURN generate_ethernet_pc_id(vector<array<uint8_t, 6>> &data, bool use_mac) {
	OsAdapterInfo *adapterInfos;
	size_t defined_adapters, adapters = 0;

	FUNCTION_RETURN result_adapterInfos = getAdapterInfos(NULL, &adapters);
	if (result_adapterInfos != FUNC_RET_BUFFER_TOO_SMALL) {
		return result_adapterInfos;
	}
	if (adapters == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	defined_adapters = adapters;
	data.reserve(adapters);
	adapterInfos = static_cast<OsAdapterInfo *>(malloc(adapters * sizeof(OsAdapterInfo)));
	result_adapterInfos = getAdapterInfos(adapterInfos, &adapters);
	if (result_adapterInfos == FUNC_RET_OK) {
		unsigned int j;
		for (j = 0; j < adapters; j++) {
			unsigned int k;
			array<uint8_t, 6> identifier;
			for (k = 0; k < 6; k++) {
				if (use_mac) {
					identifier[k] = adapterInfos[j].mac_address[k + 2];
				} else {
					// use ip
					if (k < 4) {
						identifier[k] = adapterInfos[j].ipv4_address[k];
					} else {
						// padding
						identifier[k] = 42;
					}
				}
			}
			identifier[6] = identifier[6] & 0x1F;
			data.push_back(identifier);
		}
	}
	free(adapterInfos);
	return result_adapterInfos;
}

Ethernet::Ethernet(bool useIp) : use_ip(useIp) {}

Ethernet::~Ethernet() {}

LCC_API_IDENTIFICATION_STRATEGY Ethernet::identification_strategy() const { return STRATEGY_ETHERNET; }

FUNCTION_RETURN Ethernet::identify_pc(PcIdentifier &pc_id) const {
	vector<array<uint8_t, 6>> data;
	FUNCTION_RETURN result = generate_ethernet_pc_id(data, use_ip);
	if (result == FUNC_RET_OK) {
		pc_id.set_data(data[0]);
	}
	return result;
}

std::vector<PcIdentifier> Ethernet::alternative_ids() const {
	vector<array<uint8_t, 6>> data;
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
	vector<array<uint8_t, 6>> data;
	FUNCTION_RETURN generate_ethernet = generate_ethernet_pc_id(data, use_ip);
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	if (generate_ethernet == FUNC_RET_OK) {
		// fixme
		// result = validate_identifier(identifier, data);
	}
	return result;
}

} /* namespace license */
