/*
 * ExplicitDefinition.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <string>
#include <vector>
#include <stdexcept>
#include <licensecc/datatypes.h>

#include "../base/base64.h"
#include "../base/string_utils.h"

#include "ExternalDefinition.hpp"
#include "../base/file_utils.hpp"

namespace license {
namespace locate {
using namespace std;

ExternalDefinition::ExternalDefinition(const LicenseLocation* location)
	: LocatorStrategy("ExternalDefinition"), m_location(location) {}

ExternalDefinition::~ExternalDefinition() {}

const LCC_EVENT_TYPE ExternalDefinition::license_locations(std::vector<std::string>& license_location_out) {
	LCC_EVENT_TYPE result = LICENSE_FILE_NOT_FOUND;
	if (m_location->licenseData[0] != '\0') {
		switch (m_location->license_data_type) {
			case LICENSE_PATH: {
				string licData(m_location->licenseData,
							   mstrnlen_s(m_location->licenseData, LCC_API_MAX_LICENSE_DATA_LENGTH));
				const vector<string> declared_positions = license::split_string(licData, ';');
				license_location_out = license::filter_existing_files(declared_positions);
				result = license_location_out.empty() ? LICENSE_FILE_NOT_FOUND : LICENSE_FOUND;
			} break;
			case LICENSE_ENCODED:
			case LICENSE_PLAIN_DATA:
				license_location_out.push_back(get_strategy_name());
				result = LICENSE_FOUND;
				break;
			default:
				result = LICENSE_MALFORMED;
				break;
		}
	}
	return result;
}

const std::string ExternalDefinition::retrieve_license_content(const std::string& licenseLocation) const {
	if (licenseLocation == get_strategy_name()) {
		string licData(m_location->licenseData, mstrnlen_s(m_location->licenseData, LCC_API_MAX_LICENSE_DATA_LENGTH));
		if (m_location->license_data_type == LICENSE_ENCODED) {
			// FIXME what if license content is not base64
			vector<uint8_t> raw = unbase64(licData, true);
			string str = string(reinterpret_cast<char*>(raw.data()));
			return str;
		} else {
			return licData;
		}
	} else {
		return LocatorStrategy::retrieve_license_content(licenseLocation);
	}
}

std::unique_ptr<LocatorStrategy> ExternalDefinition::clone() const {
	// For ExternalDefinition, we need to handle the fact that it holds a pointer to LicenseLocation
	// We'll create a new instance with the same location data
	return std::unique_ptr<LocatorStrategy>(new ExternalDefinition(m_location));
}

} /* namespace locate */
} /* namespace license */
