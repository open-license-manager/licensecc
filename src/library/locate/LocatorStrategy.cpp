/*
 * EnvironmentVarLocation.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <build_properties.h>

#include "../base/FileUtils.hpp"
#include "LocatorStrategy.hpp"

namespace license {
namespace locate {
using namespace std;

const string LocatorStrategy::retrieveLicense(
		const string &licenseLocation) const {
	return get_file_contents(licenseLocation.c_str(), MAX_LICENSE_LENGTH);
}

}
}
