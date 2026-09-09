/*
 * EnvironmentVarLocation.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <licensecc_properties.h>

#include <licensecc/LocatorStrategy.hpp>
#include "../base/file_utils.hpp"

namespace license {
namespace locate {
using namespace std;

const LCC_EVENT_TYPE LocatorStrategy::retrieve_license_content(const string& licenseLocation,
															   string& license_content_out) const {
	const FUNCTION_RETURN ret =
		get_file_contents(licenseLocation.c_str(), LCC_API_MAX_LICENSE_DATA_LENGTH, license_content_out);
	return ret == FUNC_RET_OK ? LICENSE_FOUND : LICENSE_FILE_NOT_FOUND;
}

}  // namespace locate
}  // namespace license
