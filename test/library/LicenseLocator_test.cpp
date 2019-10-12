#define BOOST_TEST_MODULE "license_locator_test"

#include <iostream>
#include <iterator>

#include <fstream>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <stdlib.h>
#include <cstdio>

#include <build_properties.h>
#include "../../src/library/base/EventRegistry.h"
#include "../../src/library/locate/ApplicationFolder.hpp"
#include "../../src/library/locate/EnvironmentVarLocation.hpp"
#include "../../src/library/locate/ExternalDefinition.hpp"

#define MOCK_LICENSE PROJECT_TEST_SRC_DIR "/library/test_reader.ini"

namespace test {
using namespace license::locate;
using namespace std;

/*****************************************************************************
 * Application Folder tests
 *****************************************************************************/
BOOST_AUTO_TEST_CASE( read_license_near_module ) {
#ifdef _WIN32
#ifdef _DEBUG
	const string testExeFolder = PROJECT_BINARY_DIR "/test/library/Debug";
#else
	const string testExeFolder = PROJECT_BINARY_DIR "/test/library/Release";
#endif
	const string testExe = testExeFolder + "/" + BOOST_TEST_MODULE ".exe";
#else
	const string testExeFolder = PROJECT_BINARY_DIR "/test/library";
	const string testExe = testExeFolder + "/" + BOOST_TEST_MODULE;
#endif	
	const string referenceLicenseFileName = testExeFolder + "/"
			+ BOOST_TEST_MODULE ".lic";

	//Verify we're pointing the correct executable
	ifstream f(testExe.c_str());
	BOOST_REQUIRE_MESSAGE(f.good(), "File [" + testExe + "] NOT found");

	//copy test license near module
	std::ifstream src(MOCK_LICENSE, std::ios::binary);
	std::ofstream dst(referenceLicenseFileName, std::ios::binary);
	dst << src.rdbuf();
	dst.close();

	license::EventRegistry registry;
	ApplicationFolder applicationFolder;
	vector<string> licenseInfos = applicationFolder.licenseLocations(registry);
	BOOST_CHECK(registry.isGood());
	BOOST_REQUIRE_EQUAL(1, licenseInfos.size());
	string currentLocation = licenseInfos[0];
	BOOST_CHECK_MESSAGE(referenceLicenseFileName.compare(currentLocation) == 0,
			"file found at expected location");
	string licenseRealContent = applicationFolder.retrieveLicense(
			currentLocation);
	src.seekg(0, ios::beg);
	std::string referenceContent((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());
	BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0,
			"File content is same");
	remove(referenceLicenseFileName.c_str());
}

/*****************************************************************************
 * External_Definition tests
 *****************************************************************************/

BOOST_AUTO_TEST_CASE( external_definition ) {
	//an application can define multiple license locations separated by ';'
	const char *applicationDefinedString =
	MOCK_LICENSE ";/this/one/doesnt/exist";

	//read test license
	std::ifstream src(MOCK_LICENSE, std::ios::binary);
	std::string referenceContent((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());
	license::EventRegistry registry;
	ExternalDefinition externalDefinition(applicationDefinedString);
	vector<string> licenseInfos = externalDefinition.licenseLocations(registry);
	BOOST_CHECK(registry.isGood());
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
	string currentLocation = licenseInfos[0];
	BOOST_CHECK_MESSAGE(string(MOCK_LICENSE).compare(currentLocation) == 0,
			"file found at expected location");
	string licenseRealContent = externalDefinition.retrieveLicense(
			currentLocation);
	BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0,
			"File content is same");
}

/**
 * The license file doesn't exist. Chech that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE( external_definition_not_found ) {
	const char *applicationDefinedString = PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
	license::EventRegistry registry;
	ExternalDefinition externalDefinition(applicationDefinedString);
	vector<string> licenseInfos = externalDefinition.licenseLocations(registry);

	BOOST_CHECK_MESSAGE(registry.isGood(),
			"No fatal error for now, only warnings");
	registry.turnEventIntoError(LICENSE_FILE_NOT_FOUND);
	BOOST_REQUIRE_MESSAGE(!registry.isGood(), "Error detected");
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_CHECK_MESSAGE(
			registry.getLastFailure()->event_type == LICENSE_FILE_NOT_FOUND,
			"Error detected");

}

/*****************************************************************************
 * EnvironmentVarLocation tests
 *****************************************************************************/
BOOST_AUTO_TEST_CASE( environment_var_location ) {
	//an application can define multiple license locations separated by ';'
	const char *environment_variable_value =
	MOCK_LICENSE ";/this/one/doesnt/exist";
#ifdef _WIN32
	_putenv_s(LICENSE_LOCATION_ENV_VAR, environment_variable_value);
#else
	setenv(LICENSE_LOCATION_ENV_VAR, environment_variable_value, 1);
#endif
	//read test license
	std::ifstream src(MOCK_LICENSE, std::ios::binary);
	std::string referenceContent((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());
	license::EventRegistry registry;

	EnvironmentVarLocation envVarLocationStrategy;
	vector<string> licenseInfos = envVarLocationStrategy.licenseLocations(
			registry);
	BOOST_CHECK(registry.isGood());
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
	string currentLocation = licenseInfos[0];
	BOOST_CHECK_MESSAGE(string(MOCK_LICENSE).compare(currentLocation) == 0,
			"file found at expected location");
	string licenseRealContent = envVarLocationStrategy.retrieveLicense(
			currentLocation);
	BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0,
			"File content is same");
#ifdef _WIN32
	_putenv_s(LICENSE_LOCATION_ENV_VAR, "");
#else
	unsetenv(LICENSE_LOCATION_ENV_VAR);
#endif
	
}

/**
 * The license file doesn't exist. Check that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE( environment_var_location_not_found ) {
	const char *environment_variable_value =
	PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
#ifdef _WIN32
	_putenv_s(LICENSE_LOCATION_ENV_VAR, environment_variable_value);
#else
	setenv(LICENSE_LOCATION_ENV_VAR, environment_variable_value, 1);
#endif
	license::EventRegistry registry;
	EnvironmentVarLocation envVarLocationStrategy;
	vector<string> licenseInfos = envVarLocationStrategy.licenseLocations(
			registry);
	BOOST_CHECK_MESSAGE(registry.isGood(),
			"No fatal error for now, only warnings");
	registry.turnEventIntoError(LICENSE_FILE_NOT_FOUND);
	BOOST_REQUIRE_MESSAGE(!registry.isGood(), "Error detected");
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_CHECK_MESSAGE(
			registry.getLastFailure()->event_type == LICENSE_FILE_NOT_FOUND,
			"Error detected");
#ifdef _WIN32
	_putenv_s(LICENSE_LOCATION_ENV_VAR, "");
#else
	unsetenv(LICENSE_LOCATION_ENV_VAR);
#endif
}

/**
 * The license file doesn't exist. Check that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE( environment_var_location_not_defined ) {
#ifdef _WIN32
	_putenv_s(LICENSE_LOCATION_ENV_VAR, "");
#else
	setenv(LICENSE_LOCATION_ENV_VAR, environment_variable_value, 1);
#endif
	license::EventRegistry registry;
	EnvironmentVarLocation environmentVarLocation;
	vector<string> licenseInfos = environmentVarLocation.licenseLocations(
			registry);

	BOOST_CHECK_MESSAGE(registry.isGood(),
			"No fatal error for now, only warnings");
	registry.turnEventIntoError(ENVIRONMENT_VARIABLE_NOT_DEFINED);
	BOOST_REQUIRE_MESSAGE(!registry.isGood(), "Error detected");
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_CHECK_MESSAGE(
			registry.getLastFailure()->event_type
					== ENVIRONMENT_VARIABLE_NOT_DEFINED, "Error detected");

}

}  //namespace test
