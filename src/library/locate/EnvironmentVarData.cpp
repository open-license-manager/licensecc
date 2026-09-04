/*
 * EnvironmentVarData.cpp
 *
 *  Created on: Oct 12, 2019
 *     Author: Gabriele Contini
 */

#include "EnvironmentVarData.hpp"
#include <licensecc/datatypes.h>

#include <licensecc_properties.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "../base/base64.h"
#include "../base/string_utils.h"

namespace license {
namespace locate {

using namespace std;

EnvironmentVarData::EnvironmentVarData() : LocatorStrategy("EnvironmentVarData"), isBase64(false) {}

EnvironmentVarData::~EnvironmentVarData() {}

const LCC_EVENT_TYPE EnvironmentVarData::license_locations(std::vector<std::string>& license_location_out) {
	char* env_var_value = getenv(LCC_LICENSE_DATA_ENV_VAR);
	if (env_var_value != nullptr && env_var_value[0] != '\0') {
		FILE_FORMAT licenseFormat = identify_format(env_var_value);
		if (licenseFormat == UNKNOWN) {
			return LICENSE_MALFORMED;
		} else {
			license_location_out.push_back(LCC_LICENSE_DATA_ENV_VAR);
			isBase64 = (licenseFormat == BASE64);
			return LICENSE_FOUND;
		}
	}
	return ENVIRONMENT_VARIABLE_NOT_DEFINED;
}

const std::string EnvironmentVarData::retrieve_license_content(const std::string& licenseLocation) const {
	string env_val = getenv(licenseLocation.c_str());
	if (isBase64) {
		vector<uint8_t> data = unbase64(env_val, true);
		string str = string(reinterpret_cast<char*>(data.data()));
		return str;
	}
	return env_val;
}

std::unique_ptr<LocatorStrategy> EnvironmentVarData::clone() const {
	return std::unique_ptr<EnvironmentVarData>(new EnvironmentVarData(*this));
}

}  // namespace locate
}  // namespace license
