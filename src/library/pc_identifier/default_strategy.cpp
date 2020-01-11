/*
 * default_strategy.cpp
 *
 *  Created on: Jan 1, 2020
 *      Author: devel
 */

#include <vector>
#include "pc_identifier_facade.hpp"
#include "../os/execution_environment.hpp"
#include "default_strategy.hpp"

using namespace std;
namespace license {

static vector<LCC_API_IDENTIFICATION_STRATEGY> available_strategies() {
	ExecutionEnvironment exec;
	VIRTUALIZATION virtualization = exec.getVirtualization();
	vector<LCC_API_IDENTIFICATION_STRATEGY> strategy_to_try;
	if (virtualization == CONTAINER) {
		if (exec.is_docker()) {
			strategy_to_try = LCC_DOCKER_STRATEGIES;
		} else {
			strategy_to_try = LCC_LXC_STRATEGIES;
		}
	} else if (virtualization == VM) {
		if (exec.is_cloud()) {
			strategy_to_try = LCC_CLOUD_STRATEGIES;
		} else {
			strategy_to_try = LCC_VM_STRATEGIES;
		}
	} else {
		strategy_to_try = LCC_BARE_TO_METAL_STRATEGIES;
	}
	return strategy_to_try;
}

DefaultStrategy::DefaultStrategy() {}

DefaultStrategy::~DefaultStrategy() {}

LCC_API_IDENTIFICATION_STRATEGY DefaultStrategy::identification_strategy() const { return STRATEGY_DEFAULT; }

FUNCTION_RETURN DefaultStrategy::identify_pc(PcIdentifier& pc_id) const {
	vector<LCC_API_IDENTIFICATION_STRATEGY> strategy_to_try = available_strategies();
	FUNCTION_RETURN ret = FUNC_RET_NOT_AVAIL;
	for (auto it : strategy_to_try) {
		LCC_API_IDENTIFICATION_STRATEGY strat_to_try = it;
		auto strategy_ptr = PcIdentifierFacade::STRATEGY_MAP.find(strat_to_try);
		if (strategy_ptr == PcIdentifierFacade::STRATEGY_MAP.end()) {
			throw logic_error("strategy not found");
		}
		ret = strategy_ptr->second->identify_pc(pc_id);
		if (ret == FUNC_RET_OK) {
			break;
		}
	}
	return ret;
}

std::vector<PcIdentifier> DefaultStrategy::alternative_ids() const {
	vector<LCC_API_IDENTIFICATION_STRATEGY> strategy_to_try = available_strategies();
	vector<PcIdentifier> identifiers;
	FUNCTION_RETURN ret = FUNC_RET_NOT_AVAIL;
	for (auto it : strategy_to_try) {
		LCC_API_IDENTIFICATION_STRATEGY strat_to_try = it;
		auto strategy_ptr = PcIdentifierFacade::STRATEGY_MAP.find(strat_to_try);
		if (strategy_ptr == PcIdentifierFacade::STRATEGY_MAP.end()) {
			throw logic_error("strategy not found");
		}
		vector<PcIdentifier> alt_ids = strategy_ptr->second->alternative_ids();
		// identifiers.push_back(alt_ids);
	}
	return identifiers;
}

LCC_EVENT_TYPE DefaultStrategy::validate_identifier(const PcIdentifier& identifier) const {
	// default strategy should always realize itself as a concrete strategy
	return IDENTIFIERS_MISMATCH;
}

} /* namespace license */
