#define BOOST_TEST_MODULE standard_license_test
//#define BOOST_TEST_MAIN
//#undef BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <cstring>
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

	IDENTIFICATION_STRATEGY strategy = IDENTIFICATION_STRATEGY::ETHERNET;
	FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out,
			strategy);
	BOOST_ASSERT(generate_ok == FUNCTION_RETURN::FUNC_RET_OK);
	cout << "Identifier:" << identifier_out << endl;
	vector<string> extraArgs = { "-s", identifier_out };
	generate_license(licLocation, extraArgs);

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

static void generate_reference_file(const string& idfileLocation,
		IDENTIFICATION_STRATEGY strategies[], int num_strategies) {
	ofstream idfile(idfileLocation);
	PcSignature identifier_out;
	for (int i = 0; i < num_strategies; i++) {
		FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out,
				strategies[i]);
		BOOST_ASSERT(generate_ok == FUNC_RET_OK);
		idfile << identifier_out << endl;
	}
	idfile.close();
}

BOOST_AUTO_TEST_CASE(generated_identifiers_stability) {
	const string idfileLocation(PROJECT_TEST_TEMP_DIR "/identifiers_file");
	IDENTIFICATION_STRATEGY strategies[] =
			{ DEFAULT,
					DISK_LABEL,
					DISK_NUM,
					ETHERNET };
	const int num_strategies = sizeof(strategies) / sizeof(strategies[0]);
	std::ifstream test_idfile_exist(idfileLocation);
	if (!test_idfile_exist.good()) {
		generate_reference_file(idfileLocation, strategies, num_strategies);
	}
	std::ifstream is(idfileLocation);
	std::istream_iterator<string> start(is), end;
	std::vector<string> reference_signatures(start, end);
	BOOST_ASSERT(reference_signatures.size() == num_strategies);
	PcSignature generated_identifier;
	BOOST_CHECKPOINT("Generating current signatures and comparing with past");
	for (int i = 0; i < num_strategies; i++) {
		FUNCTION_RETURN generate_ok = generate_user_pc_signature(
				generated_identifier, strategies[i]);
		BOOST_ASSERT(generate_ok == FUNCTION_RETURN::FUNC_RET_OK);
		if (reference_signatures[i] != generated_identifier) {
			string message = string("pc signature compare fail: strategy:")
					+ to_string((long double) strategies[i]) + " generated: ["
					+ generated_identifier + "] reference: ["
					+ reference_signatures[i] + "]";
			BOOST_FAIL(message);
		}
	}

	BOOST_CHECKPOINT("Verifying signatures");
	for (int j = 0; j < 100; j++) {
		for (unsigned int i = 0; i < reference_signatures.size(); i++) {
			PcSignature pcsig;
			strncpy(pcsig, reference_signatures[i].c_str(),
					sizeof(PcSignature));
			EVENT_TYPE val_result = validate_pc_signature(pcsig);
			string message = string("pc signature verification strategy:")
					+ to_string(i) + " generated: [" + generated_identifier
					+ "] reference: [" + reference_signatures[i] + "]";
			BOOST_CHECKPOINT("Verifying signature: ");
			BOOST_CHECK_EQUAL(val_result, LICENSE_OK);
		}
	}
}

