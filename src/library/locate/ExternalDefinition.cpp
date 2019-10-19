/*
 * ExplicitDefinition.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>

#include "../api/datatypes.h"
#include "../base/base64.h"
#include "../base/EventRegistry.h"
#include "../base/FileUtils.hpp"
#include "../base/StringUtils.h"

#include "ExternalDefinition.hpp"

namespace license {
namespace locate {
using namespace std;

ExternalDefinition::ExternalDefinition(const LicenseLocation *location) :
		LocatorStrategy("ExternalDefinition"), m_location(location) {
}

ExternalDefinition::~ExternalDefinition() {
}

const std::vector<std::string> ExternalDefinition::license_locations(
		EventRegistry &eventRegistry) {
	vector<string> existing_pos;
	if (m_location->licenseData != nullptr
			&& m_location->licenseData[0] != '\0') {
		eventRegistry.addEvent(LICENSE_SPECIFIED, get_strategy_name());
		FILE_FORMAT licenseFormat = identify_format(m_location->licenseData);

		if (licenseFormat == UNKNOWN) {
			eventRegistry.addEvent(LICENSE_MALFORMED, get_strategy_name());
		} else {
			existing_pos.push_back(get_strategy_name());
			licenseDataIsBase64 = (licenseFormat == BASE64);
		}
	}
	if (m_location->licenseFileLocation != nullptr
			&& strlen(m_location->licenseFileLocation) > 0) {
		const vector<string> declared_positions = license::split_string(
				m_location->licenseFileLocation, ';');
		existing_pos = license::filter_existing_files(declared_positions,
				eventRegistry, get_strategy_name().c_str());
	}
	return existing_pos;
}

const std::string ExternalDefinition::retrieve_license_content(
		const std::string &licenseLocation) const {
	if (licenseLocation == get_strategy_name()) {
		if (licenseDataIsBase64) {
			int flen = 0;
			unsigned char *raw = unbase64(m_location->licenseData,
					strlen(m_location->licenseData), &flen);
			string str = string(reinterpret_cast<char*>(raw));
			free(raw);
			return str;
		} else {
			return m_location->licenseData;
		}
	} else {
		return LocatorStrategy::retrieve_license_content(licenseLocation);
	}
}

} /* namespace locate */
} /* namespace license */
