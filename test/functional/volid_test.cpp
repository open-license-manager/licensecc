#define BOOST_TEST_MODULE standard_license_test
//#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../../src/license-generator/license-generator.h"
#include "../../../src/library/api/license++.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"
#include "../../src/library/pc-identifiers.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

BOOST_AUTO_TEST_CASE( default_volid_lic_file ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/volid_license.lic");
	PcSignature identifier_out;

	IDENTIFICATION_STRATEGY strategy = IDENTIFICATION_STRATEGY::DEFAULT;
	FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out,
			strategy);
	BOOST_ASSERT(generate_ok == FUNCTION_RETURN::OK);
	cout << "Identifier:" << identifier_out << endl;
	vector<string> extraArgs = { "-s", identifier_out };
	generate_license(licLocation, extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.openFileNearModule = false;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.environmentVariableName = "";
	EVENT_TYPE result = acquire_license("TEST", licenseLocation, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, true);
}

