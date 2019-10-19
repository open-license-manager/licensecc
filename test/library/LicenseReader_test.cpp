#define BOOST_TEST_MODULE "test_license_reader"
#include <boost/assert.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <iostream>
#include <vector>

#include "../../src/library/api/datatypes.h"
#include "../../src/library/base/EventRegistry.h"
#include "../../src/library/os/os.h"


#include <boost/test/unit_test.hpp>
#include <stdlib.h>

#include <build_properties.h>
#include "../../src/library/LicenseReader.hpp"

namespace test {
using namespace license;
using namespace std;
/**
 * Read license at fixed location
 */
BOOST_AUTO_TEST_CASE( read_single_file ) {
	const char *licLocation = PROJECT_TEST_SRC_DIR "/library/test_reader.ini";

	const LicenseLocation location = { licLocation, nullptr };
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PrODUCT",
			licenseInfos);
	BOOST_CHECK(registry.isGood());
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
}

/**
 * Test the error return if the product code is not found in the license
 */
BOOST_AUTO_TEST_CASE( product_not_licensed ) {
	const char *licLocation =
	PROJECT_TEST_SRC_DIR "/library/test_reader.ini";
	const LicenseLocation location = { licLocation, nullptr };
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT-NOT",
			licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure()!=NULL);
	BOOST_CHECK_EQUAL(PRODUCT_NOT_LICENSED,
			registry.getLastFailure()->event_type);
}

/**
 * Test the error code if the license file is specified but doesn't exists
 */
BOOST_AUTO_TEST_CASE( file_not_found ) {
	const char *licLocation = PROJECT_TEST_SRC_DIR "/library/not_found.ini";
	//const char * envName = "MYVAR";
	const LicenseLocation location = { licLocation, nullptr };
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT",
			licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure()!=NULL);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND,
			registry.getLastFailure()->event_type);
}

/**
 * Test the error code if the license default environment variable isn't specified
 */
BOOST_AUTO_TEST_CASE( env_var_not_defined ) {
	UNSETENV(LICENSE_LOCATION_ENV_VAR);
	const LicenseLocation location = { nullptr, nullptr };
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT",
			licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure()!=NULL);
	BOOST_CHECK_EQUAL(ENVIRONMENT_VARIABLE_NOT_DEFINED,
			registry.getLastFailure()->event_type);
}

/**
 * Test the error code if the license default environment variable is
 * specified but points to a non existent file.
 */
BOOST_AUTO_TEST_CASE( env_var_point_to_wrong_file ) {
	const char *environment_variable_value =
	PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
	SETENV(LICENSE_LOCATION_ENV_VAR, environment_variable_value)

	const LicenseLocation location = { nullptr, nullptr };
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT",
			licenseInfos);
	cout << registry << endl;
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure()!=NULL);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND,
			registry.getLastFailure()->event_type);
	UNSETENV(LICENSE_LOCATION_ENV_VAR);
}
} /* namespace test*/
