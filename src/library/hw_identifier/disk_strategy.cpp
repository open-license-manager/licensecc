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
namespace hw_identifier {

static FUNCTION_RETURN generate_disk_pc_id(vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> &v_disk_id,
										   bool use_id) {
	size_t disk_num = 0;
	size_t available_disk_info = 0;
	FUNCTION_RETURN result_diskinfos;

	result_diskinfos = getDiskInfos(nullptr, &disk_num);
	if (result_diskinfos != FUNC_RET_OK && result_diskinfos != FUNC_RET_BUFFER_TOO_SMALL) {
		return result_diskinfos;
	}
	if (disk_num == 0) {
		return FUNC_RET_NOT_AVAIL;
	}
	size_t mem = disk_num * sizeof(DiskInfo);
	DiskInfo *diskInfos = (DiskInfo *)malloc(mem);
	if (diskInfos == nullptr) {
		return FUNC_RET_NOT_AVAIL;
	}
	memset(diskInfos, 0, mem);
	result_diskinfos = getDiskInfos(diskInfos, &disk_num);

	if (result_diskinfos != FUNC_RET_OK) {
		free(diskInfos);
		return result_diskinfos;
	}
	for (unsigned int i = 0; i < disk_num; i++) {
		char firstChar = use_id ? diskInfos[i].label[0] : diskInfos[i].disk_sn[0];
		available_disk_info += firstChar == 0 ? 0 : 1;
	}
	if (available_disk_info == 0) {
		free(diskInfos);
		return FUNC_RET_NOT_AVAIL;
	}
	v_disk_id.reserve(available_disk_info);
	for (int j = 0; j < 2; j++) {
		int preferred = (j == 0 ? 1 : 0);
		for (unsigned int i = 0; i < disk_num; i++) {
			array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> a_disk_id;
			a_disk_id.fill(0);
			if (use_id) {
				if (diskInfos[i].disk_sn[0] != 0 && diskInfos[i].preferred == preferred) {
					size_t size = min(a_disk_id.size(), sizeof(&diskInfos[i].disk_sn));
					memcpy(&a_disk_id[0], diskInfos[i].disk_sn, size);
					v_disk_id.push_back(a_disk_id);
				}
			} else {
				if (diskInfos[i].label[0] != 0 && diskInfos[i].preferred == preferred) {
					strncpy((char *)&a_disk_id[0], diskInfos[i].label, a_disk_id.size() - 1);
					v_disk_id.push_back(a_disk_id);
				}
			}
		}
	}
	free(diskInfos);
	return FUNC_RET_OK;
}

DiskStrategy::DiskStrategy(bool use_id) : m_use_id(use_id) {}

DiskStrategy::~DiskStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY DiskStrategy::identification_strategy() const {
	return m_use_id ? STRATEGY_DISK_NUM : STRATEGY_DISK_LABEL;
}

std::vector<HwIdentifier> DiskStrategy::alternative_ids() const {
	vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN result = generate_disk_pc_id(data, m_use_id);
	vector<HwIdentifier> identifiers;
	if (result == FUNC_RET_OK) {
		identifiers.reserve(data.size());
		for (auto &it : data) {
			HwIdentifier pc_id;
			pc_id.set_identification_strategy(identification_strategy());
			pc_id.set_data(it);
			identifiers.push_back(pc_id);
		}
	}
	return identifiers;
}

}  // namespace hw_identifier
} /* namespace license */
