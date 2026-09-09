#define BOOST_TEST_MODULE "test_license_locator_strategies"
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

#include <iostream>
#include <iterator>
#include <cstdio>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/test/unit_test.hpp>
#include <stdlib.h>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include "../../src/library/os/os.h"
#include "../../src/library/locate/ApplicationFolder.hpp"
#include "../../src/library/locate/EnvironmentVarLocation.hpp"
#include "../../src/library/locate/ExternalDefinition.hpp"

#define MOCK_LICENSE PROJECT_TEST_SRC_DIR "/library/test_reader.ini"

namespace test {
using namespace license::locate;
using namespace std;
using namespace boost::filesystem;

static boost::optional<path> find_file(const path& dir_path, const path& file_name) {
	const recursive_directory_iterator end;
	const auto it = find_if(recursive_directory_iterator(dir_path), end,
							[&file_name](const directory_entry& e) { return e.path().filename() == file_name; });
	return it == end ? boost::optional<path>() : it->path();
}

/*****************************************************************************
 * Application Folder tests
 *****************************************************************************/
BOOST_AUTO_TEST_CASE(read_license_near_module) {
	const string testExeFolder = PROJECT_BINARY_DIR "/test/library";
	bool exeFileFound = false;
	string referenceExeFileName;
	string referenceLicenseFileName;
	// Verify we're pointing the correct executable, in windows isn't clear where it's built
#ifdef _WIN32
	boost::optional<path> exeLocation(find_file(path(testExeFolder), path(BOOST_TEST_MODULE ".exe")));
	if (exeLocation) {
		exeFileFound = true;
		referenceExeFileName = exeLocation.get().string();
		referenceLicenseFileName =
			referenceExeFileName.replace(referenceExeFileName.find(BOOST_TEST_MODULE ".exe"),
										 string(BOOST_TEST_MODULE ".exe").size(), BOOST_TEST_MODULE ".lic");
	}
#else
	referenceExeFileName = testExeFolder + "/" + BOOST_TEST_MODULE;
	std::ifstream f(referenceExeFileName.c_str());
	exeFileFound = f.good();
	referenceLicenseFileName = testExeFolder + "/" + BOOST_TEST_MODULE ".lic";
#endif
	BOOST_WARN_MESSAGE(!exeFileFound, "File [" + referenceExeFileName + "] NOT found");
	if (exeFileFound) {
		// copy test license near module
		std::ifstream src(MOCK_LICENSE, std::ios::binary);
		std::ofstream dst(referenceLicenseFileName, std::ios::binary);
		dst << src.rdbuf();
		dst.close();

		ApplicationFolder applicationFolder;
		vector<string> licenseInfos;
		const LCC_EVENT_TYPE ret = applicationFolder.license_locations(licenseInfos);
		BOOST_CHECK_EQUAL(LICENSE_FOUND, ret);
		BOOST_REQUIRE_EQUAL(1, licenseInfos.size());
		string currentLocation = licenseInfos[0];
		BOOST_CHECK_MESSAGE(equivalent(path(referenceLicenseFileName), path(currentLocation)),
							"file " + currentLocation + "found at expected location");
		string licenseRealContent;
		const LCC_EVENT_TYPE retrieve_ret =
			applicationFolder.retrieve_license_content(currentLocation, licenseRealContent);
		BOOST_CHECK_EQUAL(LICENSE_FOUND, retrieve_ret);
		src.seekg(0, ios::beg);
		std::string referenceContent((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
		BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0, "File content is same");
		remove(referenceLicenseFileName.c_str());
	}
}

/*****************************************************************************
 * External_Definition tests
 *****************************************************************************/

BOOST_AUTO_TEST_CASE(external_definition) {
	// an application can define multiple license locations separated by ';'
	string applicationDefinedString = MOCK_LICENSE ";/this/one/doesnt/exist";

	// read test license
	std::ifstream src(MOCK_LICENSE, std::ios::binary);
	std::string referenceContent((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(applicationDefinedString.begin(), applicationDefinedString.end(), licLocation.licenseData);
	ExternalDefinition externalDefinition(&licLocation);
	vector<string> licenseInfos;
	const LCC_EVENT_TYPE ret = externalDefinition.license_locations(licenseInfos);
	BOOST_CHECK_EQUAL(LICENSE_FOUND, ret);
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
	string currentLocation = licenseInfos[0];
	BOOST_CHECK_MESSAGE(string(MOCK_LICENSE).compare(currentLocation) == 0, "file found at expected location");
	string licenseRealContent;
	const LCC_EVENT_TYPE retrieve_ret =
		externalDefinition.retrieve_license_content(currentLocation, licenseRealContent);
	BOOST_CHECK_EQUAL(LICENSE_FOUND, retrieve_ret);
	BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0, "File content is same");
}

/**
 * The license file doesn't exist. Check that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE(external_definition_not_found) {
	string applicationDefinedString = PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(applicationDefinedString.begin(), applicationDefinedString.end(), licLocation.licenseData);
	ExternalDefinition externalDefinition(&licLocation);
	vector<string> licenseInfos;
	const LCC_EVENT_TYPE ret = externalDefinition.license_locations(licenseInfos);

	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND, ret);
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
}

/*****************************************************************************
 * EnvironmentVarLocation tests
 *****************************************************************************/
BOOST_AUTO_TEST_CASE(environment_var_location) {
	// an application can define multiple license locations separated by ';'
	const char* environment_variable_value = MOCK_LICENSE ";/this/one/doesnt/exist";
#ifdef _WIN32
	_putenv_s(LCC_LICENSE_LOCATION_ENV_VAR, environment_variable_value);
#else
	setenv(LCC_LICENSE_LOCATION_ENV_VAR, environment_variable_value, 1);
#endif
	// read test license
	std::ifstream src(MOCK_LICENSE, std::ios::binary);
	std::string referenceContent((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
	EnvironmentVarLocation envVarLocationStrategy;
	vector<string> licenseInfos;
	const LCC_EVENT_TYPE ret = envVarLocationStrategy.license_locations(licenseInfos);
	BOOST_CHECK_EQUAL(LICENSE_FOUND, ret);
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
	string currentLocation = licenseInfos[0];
	BOOST_CHECK_MESSAGE(string(MOCK_LICENSE).compare(currentLocation) == 0, "file found at expected location");
	string licenseRealContent;
	const LCC_EVENT_TYPE retrieve_ret =
		envVarLocationStrategy.retrieve_license_content(currentLocation, licenseRealContent);
	BOOST_CHECK_EQUAL(LICENSE_FOUND, retrieve_ret);
	BOOST_CHECK_MESSAGE(referenceContent.compare(licenseRealContent) == 0, "File content is same");
	UNSETENV(LCC_LICENSE_LOCATION_ENV_VAR);
}

/**
 * The license file doesn't exist. Check that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE(environment_var_location_not_found) {
	const char* environment_variable_value = PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
	SETENV(LCC_LICENSE_LOCATION_ENV_VAR, environment_variable_value);

	EnvironmentVarLocation envVarLocationStrategy;
	vector<string> licenseInfos;
	const LCC_EVENT_TYPE ret = envVarLocationStrategy.license_locations(licenseInfos);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND, ret);
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	UNSETENV(LCC_LICENSE_LOCATION_ENV_VAR);
}

/**
 * The environment variable is not defined. Check that the locator reports the right error
 */
BOOST_AUTO_TEST_CASE(environment_var_location_not_defined) {
	UNSETENV(LCC_LICENSE_LOCATION_ENV_VAR);
	EnvironmentVarLocation environmentVarLocation;
	vector<string> licenseInfos;
	const LCC_EVENT_TYPE ret = environmentVarLocation.license_locations(licenseInfos);

	BOOST_CHECK_EQUAL(ENVIRONMENT_VARIABLE_NOT_DEFINED, ret);
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
}
}  // namespace test
