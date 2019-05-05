#define BOOST_TEST_MODULE license_generator_test
//#define BOOST_TEST_MAIN
//#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../src/tools/license-generator/license-generator.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../../src/library/ini/SimpleIni.h"

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
	delete[] (argv);
	BOOST_CHECK_EQUAL(retCode, 0);
	BOOST_ASSERT(fs::exists(fname));
	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile(fname.c_str());
	BOOST_CHECK_GE(rc,0);
	int sectionSize = ini.GetSectionSize(prod_name.c_str());
	BOOST_CHECK_GT(sectionSize,0);
}

BOOST_AUTO_TEST_CASE( generate_lic_file ) {
	const string licLocation(PROJECT_TEST_TEMP_DIR "/test1.lic");
	generate_license(string("TEST"), licLocation);
}

