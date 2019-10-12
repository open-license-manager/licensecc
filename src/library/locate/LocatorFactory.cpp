/*
 * LocatorFactory.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: Gabriele Contini
 */

#include "build_properties.h"

#include "LocatorStrategy.hpp"
#include "LocatorFactory.hpp"
#include "ApplicationFolder.hpp"
//#include "EnvironmentVarData.hpp"
#include "EnvironmentVarLocation.hpp"
#include "ExternalDefinition.hpp"

namespace license {
namespace locate {

using namespace std;

FUNCTION_RETURN LocatorFactory::getActiveStrategies(
		vector<unique_ptr<LocatorStrategy>> &strategies,
		const char *locationHint) {
#if(FIND_LICENSE_NEAR_MODULE)
	strategies.push_back(
			unique_ptr<LocatorStrategy>(
					(LocatorStrategy*) new ApplicationFolder()));
#endif
#if(FIND_LICENSE_WITH_ENV_VAR)
	strategies.push_back(
			unique_ptr<LocatorStrategy>(
					(LocatorStrategy*) new EnvironmentVarLocation()));
//	strategies.push_back(
//			unique_ptr<LocatorStrategy>(
//					(LocatorStrategy*) new EnvironmentVarData()));
#endif
	if (locationHint != nullptr) {
		strategies.push_back(
				unique_ptr<LocatorStrategy>(
						(LocatorStrategy*) new ExternalDefinition(locationHint)));
	}
	return strategies.size() > 0 ? FUNC_RET_OK : FUNC_RET_NOT_AVAIL;

}

}
}
