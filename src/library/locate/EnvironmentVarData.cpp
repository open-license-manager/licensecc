/*
 * EnvironmentVarData.cpp
 *
 *  Created on: Oct 12, 2019
 *     Author: Gabriele Contini
 */

#include "EnvironmentVarData.hpp"

#include <licensecc_properties.h>
#include <cstdlib>
#include <regex>
#include <string>
#include <vector>

#include "../api/datatypes.h"
#include "../base/base64.h"
#include "../base/EventRegistry.h"
#include "../base/StringUtils.h"

namespace license {
namespace locate {

using namespace std;

EnvironmentVarData::EnvironmentVarData() :
		LocatorStrategy("EnvironmentVarData") {
}

EnvironmentVarData::~EnvironmentVarData() {
}

const vector<string> EnvironmentVarData::license_locations(
		EventRegistry &eventRegistry) {
	vector<string> diskFiles;
	char *env_var_value = getenv(LICENSE_DATA_ENV_VAR);
	if (env_var_value != nullptr && env_var_value[0] != '\0') {
		eventRegistry.addEvent(LICENSE_SPECIFIED, LICENSE_LOCATION_ENV_VAR);
		FILE_FORMAT licenseFormat = identify_format(env_var_value);
		if (licenseFormat == UNKNOWN) {
			eventRegistry.addEvent(LICENSE_MALFORMED, LICENSE_LOCATION_ENV_VAR);
		} else {
			diskFiles.push_back(LICENSE_LOCATION_ENV_VAR);
			isBase64 = (licenseFormat == BASE64);
		}
	} else {
		eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED,
		LICENSE_LOCATION_ENV_VAR);
	}
	return diskFiles;
}

const std::string EnvironmentVarData::retrieve_license_content(
		const std::string &licenseLocation) const {
	string tmpVal = getenv(LICENSE_LOCATION_ENV_VAR);
	if (isBase64) {
		int flen = 0;
		unsigned char *raw = unbase64(tmpVal.c_str(), tmpVal.length(), &flen);
		string str = string(reinterpret_cast<char*>(raw));
		free(raw);
		return str;
	}
	return tmpVal;
}

}
}
