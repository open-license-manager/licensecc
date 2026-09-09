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

const LCC_EVENT_TYPE EnvironmentVarData::retrieve_license_content(const std::string& licenseLocation,
																  std::string& content_out) const {
	const char* env_val = getenv(licenseLocation.c_str());
	if (env_val == nullptr || env_val[0] == '\0') {
		content_out.clear();
		return LICENSE_FILE_NOT_FOUND;
	}
	if (isBase64) {
		vector<uint8_t> data = unbase64(env_val, true);
		content_out = string(reinterpret_cast<char*>(data.data()));
	} else {
		content_out = env_val;
	}
	return LICENSE_FOUND;
}

std::unique_ptr<LocatorStrategy> EnvironmentVarData::clone() const {
	return std::unique_ptr<EnvironmentVarData>(new EnvironmentVarData(*this));
}

}  // namespace locate
}  // namespace license
