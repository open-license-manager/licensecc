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

DiskStrategy::~DiskStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY DiskStrategy::identification_strategy() const {
	return LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK;
}

FUNCTION_RETURN DiskStrategy::alternative_ids(std::vector<HwIdentifier>& identifiers) const noexcept {
	std::vector<DiskInfo> disk_infos;
	identifiers.clear();
	FUNCTION_RETURN result_diskinfos = getDiskInfos(disk_infos);
	if (result_diskinfos != FUNC_RET_OK) {
		return FUNC_RET_NOT_AVAIL;
	}

	identifiers.reserve(disk_infos.size() * 2);
	// first preferred (SN/label) then others
	for (int j = 0; j < 2; j++) {
		bool preferred = (j == 0);
		for (size_t i = 0; i < disk_infos.size(); i++) {
			if (disk_infos[i].preferred == preferred) {
				if (disk_infos[i].sn_initialized) {
					HwIdentifier tmp_id;
					tmp_id.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK);
					tmp_id.set_data(disk_infos[i].disk_sn);
					identifiers.push_back(tmp_id);
				}
				if (disk_infos[i].label_initialized) {
					HwIdentifier tmp_id;
					tmp_id.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK);
					tmp_id.set_data(string(disk_infos[i].label));
					identifiers.push_back(tmp_id);
				}
			}
		}
	}
	return FUNC_RET_OK;
}

}  // namespace hw_identifier
} /* namespace license */
