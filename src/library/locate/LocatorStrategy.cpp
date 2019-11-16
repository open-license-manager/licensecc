/*
 * EnvironmentVarLocation.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <licensecc_properties.h>

#include "../base/FileUtils.hpp"
#include "LocatorStrategy.hpp"

namespace license {
namespace locate {
using namespace std;

const string LocatorStrategy::retrieve_license_content(const string &licenseLocation) const {
	return get_file_contents(licenseLocation.c_str(), MAX_LICENSE_LENGTH);
}

}  // namespace locate
}  // namespace license
