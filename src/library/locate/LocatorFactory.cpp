/*
 * LocatorFactory.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: Gabriele Contini
 */

#include "LocatorStrategy.hpp"
#include "LocatorFactory.hpp"
#include "ApplicationFolder.hpp"
#include "EnvironmentVarData.hpp"
#include "EnvironmentVarLocation.hpp"
#include "ExternalDefinition.hpp"

namespace license {
namespace locate {
std::vector<std::unique_ptr<LocatorStrategy>> LocatorFactory::LocatorFactory::extra_strategies;

bool LocatorFactory::find_license_near_moduleb = FIND_LICENSE_NEAR_MODULE;
bool LocatorFactory::find_license_with_env_varb = FIND_LICENSE_WITH_ENV_VAR;

// Behavior removed: get_active_strategies2 and the LocatorFactory instance constructor
// (active_strategies + event_registry members) have been deleted. The same strategy
// construction logic now lives only in get_active_strategies.

void LocatorFactory::set_extra_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies) {
	extra_strategies.clear();
	for (auto& strategy : strategies) {
		extra_strategies.push_back(strategy->clone());
	}
}

FUNCTION_RETURN LocatorFactory::get_active_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies,
													  const LicenseLocation* locationHint) {
	if (find_license_near_moduleb) {
		strategies.push_back(std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ApplicationFolder())));
	}
	if (find_license_with_env_varb) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarLocation())));
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarData())));
	}
	if (locationHint != nullptr) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ExternalDefinition(locationHint))));
	}

	for (auto& strategy : extra_strategies) {
		strategies.push_back(strategy->clone());
	}
	return strategies.size() > 0 ? FUNC_RET_OK : FUNC_RET_NOT_AVAIL;
}

// Behavior moved: the RawLicenseCursor inner class, begin() and end() have been
// extracted into the standalone FoundLicenseCursor class (FoundLicenseCursor.hpp/.cpp).

}  // namespace locate
}  // namespace license
