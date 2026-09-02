#define BOOST_TEST_MODULE test_pc_signature_verifier

#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc/datatypes.h>

#include <licensecc/EventRegistry.h>
#include "../../../src/library/LicenseParser.hpp"
#include "../../../src/library/limits/limit_verifiers.hpp"

namespace license {
namespace test {

using namespace std;

static FullLicenseInfo make_license() {
	FullLicenseInfo lic;
	lic.source = "test_license.lic";
	lic.m_project = "PRODUCT";
	lic.license_signature = "signature";
	return lic;
}

BOOST_AUTO_TEST_CASE(no_client_signature_is_valid) {
	FullLicenseInfo lic = make_license();
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_pc_signature(lic, out);

	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(out.linked_to_pc, false);
}

BOOST_AUTO_TEST_CASE(invalid_client_signature_is_error) {
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_CLIENT_SIGNATURE] = "0000-0000-0000-0000";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_pc_signature(lic, out);

	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
	BOOST_CHECK_EQUAL(out.linked_to_pc, true);
}

}  // namespace test
}  // namespace license