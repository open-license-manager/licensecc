/*
 * system_id_strategy.cpp
 *
 *  Created on: Aug 13, 2026
 *      Author: Gabriele Contini
 */

#include <vector>
#include <string>
#include "../os/os.h"
#include "system_id_strategy.hpp"
#include "../base/logger.h"

using namespace std;
namespace license {
namespace hw_identifier {

SystemIdStrategy::SystemIdStrategy() {}

SystemIdStrategy::~SystemIdStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY SystemIdStrategy::identification_strategy() const { return STRATEGY_SYSTEM_ID; }

FUNCTION_RETURN SystemIdStrategy::alternative_ids(std::vector<HwIdentifier>& identifiers) const noexcept {
	identifiers.clear();
	std::string sys_id;

	FUNCTION_RETURN ret = getOsSpecificIdentifier(sys_id);
	if (ret == FUNC_RET_OK) {
		HwIdentifier identifier;
		identifier.set_identification_strategy(identification_strategy());
		identifier.set_data(sys_id);
		identifiers.push_back(identifier);
		return FUNC_RET_OK;
	}
	return FUNC_RET_NOT_AVAIL;
}

}  // namespace hw_identifier
} /* namespace license */