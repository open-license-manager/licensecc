#define BOOST_TEST_MODULE test_standard_license

#include <boost/test/unit_test.hpp>
#include "../../src/tools/license-generator/license-generator.h"
#include "../../src/library/api/license++.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"
#include "../../src/library/base/FileUtils.hpp"

namespace test {
namespace fs = boost::filesystem;
using namespace license;
using namespace std;

BOOST_AUTO_TEST_CASE( standard_lic_file ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/standard_license.lic");
	const vector<string> extraArgs;
	generate_license(licLocation, extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.licenseData = nullptr;
	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
			&license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

/**
 * Pass the license data to the application.
 */
BOOST_AUTO_TEST_CASE( b64_environment_variable ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/standard_env_license.lic");
	const vector<string> extraArgs;
	generate_license(licLocation, extraArgs);
	const string licensestr(license::get_file_contents(licLocation.c_str(), MAX_LICENSE_LENGTH));
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = nullptr;
	licenseLocation.licenseData = licensestr.c_str();
	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
			&license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

BOOST_AUTO_TEST_CASE( pc_identifier ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/pc_identifier.lic");
	const vector<string> extraArgs = { "-s", "Jaaa-aaaa-MG9F-ZhB1" };
	generate_license(licLocation, extraArgs);

	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.licenseData = "";
	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
			&license);
	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, true);
}
}
