/*
 * pc_identifier_facade.cpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#include "pc_identifier_facade.hpp"

#include <cstdlib>
#include <stdexcept>

#include "../../../projects/DEFAULT/include/licensecc/DEFAULT/licensecc_properties.h"
#include "../base/base.h"
#include "../base/logger.h"
#include "../os/cpu_info.hpp"
#include "../os/execution_environment.hpp"
#include "default_strategy.hpp"
#include "ethernet.hpp"
#include "pc_identifier.hpp"

namespace license {
using namespace std;
static std::unordered_map<LCC_API_IDENTIFICATION_STRATEGY, std::unique_ptr<IdentificationStrategy>> init() {
	unordered_map<LCC_API_IDENTIFICATION_STRATEGY, std::unique_ptr<IdentificationStrategy>> strategy_map;
	strategy_map[STRATEGY_DEFAULT] =
		unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new DefaultStrategy()));
	strategy_map[STRATEGY_ETHERNET] =
		unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new Ethernet(false)));
	strategy_map[STRATEGY_IP_ADDRESS] =
		unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new Ethernet(true)));

	return strategy_map;
}

std::unordered_map<LCC_API_IDENTIFICATION_STRATEGY, std::unique_ptr<IdentificationStrategy>>
	PcIdentifierFacade::STRATEGY_MAP = init();

LCC_EVENT_TYPE PcIdentifierFacade::validate_pc_signature(const std::string& str_code) {
	PcIdentifier pc_id(str_code);
	LCC_API_IDENTIFICATION_STRATEGY id_strategy = pc_id.get_identification_strategy();
	auto it = STRATEGY_MAP.find(id_strategy);
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	if (it != STRATEGY_MAP.end()) {
		result = it->second->validate_identifier(pc_id);
	} else {
		// TODO: log
	}
	return result;
}

std::string PcIdentifierFacade::generate_user_pc_signature(LCC_API_IDENTIFICATION_STRATEGY strategy) {
	bool use_env_var = false;
	if (strategy == STRATEGY_DEFAULT) {
		char* env_var_value = getenv(LCC_IDENTIFICATION_STRATEGY_ENV_VAR);
		if (env_var_value != nullptr && env_var_value[0] != '\0') {
			int strategy_int = atoi(env_var_value);
			if (strategy_int < 0 || strategy_int > 3) {
				LOG_WARN("unknown " LCC_IDENTIFICATION_STRATEGY_ENV_VAR " %s", env_var_value);
			} else {
				strategy = (LCC_API_IDENTIFICATION_STRATEGY)strategy_int;
				use_env_var = true;
			}
		}
	}
	auto it = STRATEGY_MAP.find(strategy);
	PcIdentifier pc_id;
	if (it != STRATEGY_MAP.end()) {
		FUNCTION_RETURN result = it->second->identify_pc(pc_id);
		if (result != FUNC_RET_OK) {
			/// FIXME
		}
	} else {
		throw logic_error("Specified identification strategy invalid");
	}
	ExecutionEnvironment exec;
	VIRTUALIZATION virtualization = exec.getVirtualization();
	pc_id.set_virtual_environment(virtualization);
	pc_id.set_use_environment_var(use_env_var);
	if (virtualization != NONE) {
		bool isCloud = exec.is_cloud();
		if (isCloud) {
			pc_id.set_cloud_provider(exec.getCloudProvider());
		} else {
			CpuInfo cpu;
			pc_id.set_virtualization(cpu.getVirtualizationDetail());
		}
	}
	return pc_id.print();
}
} /* namespace license */
