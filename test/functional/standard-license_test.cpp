#define BOOST_TEST_MODULE standard_license_test
//#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../../src/license-generator/license-generator.h"
#include "../../../src/library/api/license++.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../../src/library/reader/SimpleIni.h"


namespace fs = boost::filesystem;
using namespace license;
using namespace std;

void generate_license(const string& prod_name, const string& fname) {
	int argc = 4;
	const char** argv = new const char*[argc + 1];
	argv[0] = "lic-generator";
	argv[1] = "-o";
	argv[2] = fname.c_str();
	argv[3] = "test";
	int retCode = LicenseGenerator::generateLicense(argc, argv);
	delete (argv);
	BOOST_CHECK_EQUAL(retCode, 0);
	BOOST_ASSERT(fs::exists(fname));
	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile(fname.c_str());
	BOOST_CHECK_GE(rc,0);
	int sectionSize = ini.GetSectionSize(prod_name.c_str());
	BOOST_CHECK_GT(sectionSize,0);
}

BOOST_AUTO_TEST_CASE( standard_lic_file ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/standard_license.lic");
	generate_license(string("TEST"), licLocation);
	/* */
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.openFileNearModule=false;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.environmentVariableName = "";
	EVENT_TYPE result = acquire_license("TEST",
			licenseLocation, & license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

