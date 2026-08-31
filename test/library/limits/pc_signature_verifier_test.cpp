#define BOOST_TEST_MODULE test_pc_signature_verifier

#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc/datatypes.h>

#include "../../../src/library/base/EventRegistry.h"
#include "../../../src/library/LicenseParser.hpp"
#include "../../../src/library/limits/pc_signature_verifier.hpp"

namespace license {
namespace test {

using namespace std;

static FullLicenseInfo make_license() {
	FullLicenseInfo lic("test_license.lic", "PRODUCT", "signature");
	return lic;
}

BOOST_AUTO_TEST_CASE(no_client_signature_is_valid) {
	PcSignatureVerifier verifier;
	FullLicenseInfo lic = make_license();
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verifier.verify_limit(lic, out);

	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(out.linked_to_pc, false);
}

BOOST_AUTO_TEST_CASE(invalid_client_signature_is_error) {
	PcSignatureVerifier verifier;
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_CLIENT_SIGNATURE] = "0000-0000-0000-0000";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verifier.verify_limit(lic, out);

	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
	BOOST_CHECK_EQUAL(out.linked_to_pc, true);
}

}  // namespace test
}  // namespace license