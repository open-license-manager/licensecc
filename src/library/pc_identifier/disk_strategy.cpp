/*
 * disk_strategy.cpp
 *
 *  Created on: Jan 14, 2020
 *      Author: devel
 */
#include <string.h>
#include "../os/os.h"
#include "disk_strategy.hpp"

using namespace std;
namespace license {

static FUNCTION_RETURN generate_disk_pc_id(vector<array<uint8_t, 6>> &v_disk_id, bool use_id) {
	size_t disk_num, available_disk_info = 0;
	FUNCTION_RETURN result_diskinfos;
	unsigned int i;
	DiskInfo *diskInfos;

	result_diskinfos = getDiskInfos(NULL, &disk_num);
	if (result_diskinfos != FUNC_RET_OK && result_diskinfos != FUNC_RET_BUFFER_TOO_SMALL) {
		return result_diskinfos;
	}
	if (disk_num == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	diskInfos = (DiskInfo *)malloc(disk_num * sizeof(DiskInfo));
	memset(diskInfos, 0, disk_num * sizeof(DiskInfo));
	result_diskinfos = getDiskInfos(diskInfos, &disk_num);
	if (result_diskinfos != FUNC_RET_OK) {
		free(diskInfos);
		return result_diskinfos;
	}
	for (i = 0; i < disk_num; i++) {
		char firstChar = use_id ? diskInfos[i].label[0] : diskInfos[i].disk_sn[0];
		available_disk_info += firstChar == 0 ? 0 : 1;
	}
	if (available_disk_info == 0) {
		free(diskInfos);
		return FUNC_RET_NOT_AVAIL;
	}
	v_disk_id.reserve(available_disk_info);
	for (i = 0; i < disk_num; i++) {
		array<uint8_t, 6> a_disk_id;
		if (use_id) {
			if (diskInfos[i].disk_sn[0] != 0) {
				memcpy(&a_disk_id[0], &diskInfos[i].disk_sn[2], a_disk_id.size());
				v_disk_id.push_back(a_disk_id);
			}
		} else {
			if (diskInfos[i].label[0] != 0) {
				a_disk_id.fill(0);
				// strncpy((&a_disk_id[0], diskInfos[i].label, a_disk_id.size());
				v_disk_id.push_back(a_disk_id);
			}
		}
	}
	free(diskInfos);
	return FUNC_RET_OK;
}

DiskStrategy::DiskStrategy(bool use_id) : m_use_id(use_id) {
}

DiskStrategy::~DiskStrategy() {
}

LCC_API_IDENTIFICATION_STRATEGY DiskStrategy::identification_strategy() const {
	return m_use_id ? STRATEGY_DISK_NUM : STRATEGY_DISK_LABEL;
}

FUNCTION_RETURN DiskStrategy::identify_pc(PcIdentifier &pc_id) const {
	vector<array<uint8_t, 6>> data;
	FUNCTION_RETURN result = generate_disk_pc_id(data, m_use_id);
	if (result == FUNC_RET_OK) {
		pc_id.set_data(data[0]);
	}
	return result;
}

std::vector<PcIdentifier> DiskStrategy::alternative_ids() const {}

LCC_EVENT_TYPE DiskStrategy::validate_identifier(const PcIdentifier &identifier) const {
	vector<array<uint8_t, 6>> data;
	FUNCTION_RETURN generate_ethernet = generate_disk_pc_id(data, m_use_id);
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	if (generate_ethernet == FUNC_RET_OK) {
		// result = const_cast<IdentificationStrategy *>(this)->validate_identifier(identifier, data);
	}
	return result;
}

} /* namespace license */
