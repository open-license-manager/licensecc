/*
 * ApplicationFolder.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */
#include <fstream>
#include <sstream>
#include <string>

#include <licensecc/datatypes.h>
#include <licensecc_properties.h>

#include "../base/logger.h"
#include "../base/base.h"
#include "../os/os.h"
#include "ApplicationFolder.hpp"
#include "../base/file_utils.hpp"

namespace license {
namespace locate {
using namespace std;

ApplicationFolder::ApplicationFolder() : LocatorStrategy("ApplicationFolder") {}

ApplicationFolder::~ApplicationFolder() {}

const LCC_EVENT_TYPE ApplicationFolder::license_locations(std::vector<std::string>& license_location_out) {
	LCC_EVENT_TYPE result = LICENSE_FILE_NOT_FOUND;
	char fname[MAX_PATH] = {0};
	const FUNCTION_RETURN fret = getModuleName(fname);
	if (fret == FUNC_RET_OK) {
		const string module_name = remove_extension(fname);
		const string temptativeLicense = string(module_name) + LCC_LICENSE_FILE_EXTENSION;
		ifstream f(temptativeLicense.c_str());
		if (f.good()) {
			license_location_out.push_back(temptativeLicense);
			result = LICENSE_FOUND;
		}
		f.close();
	} else {
		LOG_WARN("Error determining module name.");
	}
	return result;
}

std::unique_ptr<LocatorStrategy> ApplicationFolder::clone() const {
	return std::unique_ptr<LocatorStrategy>(new ApplicationFolder(*this));
}

}  // namespace locate
}  // namespace license
