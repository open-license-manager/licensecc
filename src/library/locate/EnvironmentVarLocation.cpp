/*
 * EnvironmentVarLocation.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <build_properties.h>

#include "../base/FileUtils.hpp"
#include "../base/StringUtils.h"
#include "EnvironmentVarLocation.hpp"

namespace license {
namespace locate {
using namespace std;

EnvironmentVarLocation::EnvironmentVarLocation() :
		LocatorStrategy("EnvironmentVarLocation") {
}

EnvironmentVarLocation::~EnvironmentVarLocation() {
}

const vector<string> EnvironmentVarLocation::license_locations(
		EventRegistry &eventRegistry) {
	vector<string> licenseFileFoundWithEnvVariable;

	const string varName(LICENSE_LOCATION_ENV_VAR);
	if (varName.length() > 0) {
		//var name is defined in header files.
		char *env_var_value = getenv(LICENSE_LOCATION_ENV_VAR);
		if (env_var_value != nullptr && env_var_value[0] != '\0') {
			const vector<string> declared_positions = license::split_string(
					string(env_var_value), ';');
			licenseFileFoundWithEnvVariable = license::filter_existing_files(
					declared_positions, eventRegistry, LICENSE_LOCATION_ENV_VAR);
		} else {
			eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED);
		}
	}
	return licenseFileFoundWithEnvVariable;
}

}
}
