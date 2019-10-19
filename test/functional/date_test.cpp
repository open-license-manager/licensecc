#define BOOST_TEST_MODULE date_test

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <build_properties.h>
#include "../../src/tools/license-generator/license-generator.h"
#include "../../src/library/api/license++.h"
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

namespace test {

BOOST_AUTO_TEST_CASE( license_not_expired ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/not_expired.lic");
	vector<string> extraArgs;
	extraArgs.push_back("-e");
	extraArgs.push_back("2050-10-10");
	generate_license(licLocation, extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.licenseData = "";
	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
			&license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, true);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

BOOST_AUTO_TEST_CASE( license_expired ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/expired.lic");
	remove(licLocation.c_str());
	vector<string> extraArgs;
	extraArgs.push_back("-e");
	extraArgs.push_back("2013-10-10");
	generate_license(licLocation, extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.licenseData = nullptr;
	BOOST_TEST_MESSAGE("before acquire license");
	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
			&license);
	BOOST_CHECK_EQUAL(result, PRODUCT_EXPIRED);
	BOOST_CHECK_EQUAL(license.has_expiry, true);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

}
