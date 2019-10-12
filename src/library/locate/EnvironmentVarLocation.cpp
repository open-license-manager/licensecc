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

const vector<string> EnvironmentVarLocation::licenseLocations(
		EventRegistry &eventRegistry) const {
	vector<string> licenseFileFoundWithEnvVariable;

	const string varName(LICENSE_LOCATION_ENV_VAR);
	if (varName.length() > 0) {
		//var name is defined in header files.
		char *env_var_value = getenv(LICENSE_LOCATION_ENV_VAR);
		if (env_var_value != nullptr && env_var_value[0] != '\0') {
			const vector<string> declared_positions = license::split_string(
					string(env_var_value), ';');
			vector<string> existing_pos = license::filter_existing_files(
					declared_positions);
			if (existing_pos.size() > 0) {
				for (auto it = existing_pos.begin(); it != existing_pos.end();
						++it) {
					licenseFileFoundWithEnvVariable.push_back(*it);
					eventRegistry.addEvent(LICENSE_FILE_FOUND, SVRT_INFO, *it);
				}
			} else {
				eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND, SVRT_WARN,
						env_var_value);
			}
		} else {
			eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED, SVRT_WARN);
		}
	}
	return licenseFileFoundWithEnvVariable;
}

}
}
