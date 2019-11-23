#define BOOST_TEST_MODULE test_standard_license

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <licensecc/licensecc.h>
#include <licensecc_properties_test.h>
#include <licensecc_properties.h>

#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"
#include "../../src/library/base/FileUtils.hpp"

namespace license {
namespace test {
namespace fs = boost::filesystem;
using namespace license;
using namespace std;

BOOST_AUTO_TEST_CASE( standard_lic_file ) {
	const vector<string> extraArgs;
	const string licLocation = generate_license("standard_license", extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.licenseData = nullptr;
	const EVENT_TYPE result = acquire_license(nullptr, &licenseLocation, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

/**
 * Pass the license data to the application.
 */

// BOOST_AUTO_TEST_CASE( b64_environment_variable ) {
//	const string licLocation(PROJECT_TEST_TEMP_DIR "/standard_env_license.lic");
//	const vector<string> extraArgs;
//	generate_license(licLocation, extraArgs);
//	const string licensestr(license::get_file_contents(licLocation.c_str(), MAX_LICENSE_LENGTH));
//	/* */
//	LicenseInfo license;
//	LicenseLocation licenseLocation;
//	licenseLocation.licenseFileLocation = nullptr;
//	licenseLocation.licenseData = licensestr.c_str();
//	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
//			&license);
//	BOOST_CHECK_EQUAL(result, LICENSE_OK);
//	BOOST_CHECK_EQUAL(license.has_expiry, false);
//	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
//}
//
// BOOST_AUTO_TEST_CASE( pc_identifier ) {
//	const string licLocation(PROJECT_TEST_TEMP_DIR "/pc_identifier.lic");
//	const vector<string> extraArgs = { "-s", "Jaaa-aaaa-MG9F-ZhB1" };
//	generate_license(licLocation, extraArgs);
//
//	LicenseInfo license;
//	LicenseLocation licenseLocation;
//	licenseLocation.licenseFileLocation = licLocation.c_str();
//	licenseLocation.licenseData = "";
//	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
//			&license);
//	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
//	BOOST_CHECK_EQUAL(license.has_expiry, false);
//	BOOST_CHECK_EQUAL(license.linked_to_pc, true);
//}
}
}  // namespace license
