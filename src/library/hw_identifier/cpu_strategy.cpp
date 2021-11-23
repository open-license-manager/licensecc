/*
 * cpu_strategy.cpp
 *
 *  Created on: Nov 23, 2021
 *      Author: Tibalt
 */
#include <string.h>
#include "../os/os.h"
#include "cpu_strategy.hpp"

using namespace std;
namespace license {
namespace hw_identifier {

static array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> generate_id_by_model(const uint32_t &cpu_info) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> a_disk_id = {};
	memcpy(&a_disk_id[0], &cpu_info, sizeof(cpu_info));
	return a_disk_id;
}


static FUNCTION_RETURN generate_cpu_pc_id(array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> &cpu_id) {
  uint32_t model;
	FUNCTION_RETURN result_diskinfos = getCPUModel(model);
	if (result_diskinfos != FUNC_RET_OK) {
		return result_diskinfos;
	}
	cpu_id = generate_id_by_model(model);
	return  FUNC_RET_OK ;
}

CPUStrategy::~CPUStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY CPUStrategy::identification_strategy() const {
	return LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_CPU_MODEL;
}

std::vector<HwIdentifier> CPUStrategy::alternative_ids() const {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data;
	FUNCTION_RETURN result = generate_cpu_pc_id(data);
	vector<HwIdentifier> identifiers;
	if (result == FUNC_RET_OK) {
		HwIdentifier pc_id;
		pc_id.set_identification_strategy(identification_strategy());
		pc_id.set_data(data);
		identifiers.push_back(pc_id);
	}
	return identifiers;
}

}  // namespace hw_identifier
} /* namespace license */
