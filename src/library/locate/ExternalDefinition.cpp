/*
 * ExplicitDefinition.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include "../base/StringUtils.h"
#include "../base/FileUtils.hpp"
#include "ExternalDefinition.hpp"

namespace license {
namespace locate {
using namespace std;

ExternalDefinition::ExternalDefinition(const char *location) :
		LocatorStrategy("ExternalDefinition"), m_location(location) {
}

ExternalDefinition::~ExternalDefinition() {
}

const std::vector<std::string> ExternalDefinition::licenseLocations(
		EventRegistry &eventRegistry) const {
	const vector<string> declared_positions = license::split_string(m_location,
			';');
	const vector<string> existing_pos = license::filter_existing_files(
			declared_positions);
	if (existing_pos.size() > 0) {
		for (auto it = existing_pos.begin(); it != existing_pos.end(); ++it) {
			eventRegistry.addEvent(LICENSE_FILE_FOUND, SVRT_INFO, *it);
		}
	} else {
		eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND, SVRT_WARN, m_location);
	}
	return existing_pos;
}

} /* namespace locate */
} /* namespace license */
