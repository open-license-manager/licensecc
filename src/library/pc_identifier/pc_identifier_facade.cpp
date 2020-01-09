/*
 * pc_identifier_facade.cpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */
#include <unordered_map>
#include <bits/unique_ptr.h>

#include "pc_identifier_facade.hpp"
#include "identification_strategy.hpp"
#include "pc_identifier.hpp"
#include "../os/execution_environment.hpp"
namespace license {

std::unordered_map<IDENTIFICATION_STRATEGY, std::unique_ptr<IdentificationStrategy>> STRATEGY_MAP;

EVENT_TYPE PcIdentifierFacade::validate_pc_signature(const std::string& str_code) {
	PcIdentifier pc_id(str_code);
	IDENTIFICATION_STRATEGY id_strategy = pc_id.get_identification_strategy();
	auto it = STRATEGY_MAP.find(id_strategy);
	EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	if (it != STRATEGY_MAP.end()) {
		result = it->second->validate_identifier(pc_id);
	}
	return result;
}

std::string PcIdentifierFacade::generate_user_pc_signature(IDENTIFICATION_STRATEGY strategy) { PcIdentifier pc_id; }
} /* namespace license */
