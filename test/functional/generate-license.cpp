/*
 * generate-license.c
 *
 *  Created on: Apr 13, 2014
 *      
 */

#include <boost/test/unit_test.hpp>
#include "../../src/tools/license-generator/license-generator.h"
#include <build_properties.h>
#include <boost/filesystem.hpp>
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

void generate_license(const string& fname, const vector<string>& other_args) {
	int argc = 4+other_args.size();
	const char** argv = new const char*[argc + 1];
	unsigned int i=0;
	argv[i++] = "lic-generator";
	for(;i<=other_args.size();i++){
		argv[i] = other_args[i-1].c_str();
	}
	argv[i++] = "-o";
	argv[i++] = fname.c_str();
	argv[i++] = "TEST";
	int retCode = LicenseGenerator::generateLicense(argc, argv);
	delete[] (argv);
	BOOST_CHECK_EQUAL(retCode, 0);
	BOOST_ASSERT(fs::exists(fname));
	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile(fname.c_str());
	BOOST_CHECK_GE(rc,0);
	int sectionSize = ini.GetSectionSize("TEST");
	BOOST_CHECK_GT(sectionSize,0);
}
