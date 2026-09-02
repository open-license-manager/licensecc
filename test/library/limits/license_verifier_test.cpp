#define BOOST_TEST_MODULE test_license_verifier

#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc/datatypes.h>

#include "../../../src/library/base/EventRegistry.h"
#include "../../../src/library/LicenseParser.hpp"
#include "../../../src/library/limits/license_verifier.hpp"
#include "../../../src/library/limits/limit_verifier.hpp"

namespace license {
namespace test {

using namespace std;

namespace {

const LimitVerifierFn ok_verifier = [](const FullLicenseInfo&, LicenseInfo&) -> LCC_EVENT_TYPE { return LICENSE_OK; };

const LimitVerifierFn fail_verifier = [](const FullLicenseInfo&, LicenseInfo&) -> LCC_EVENT_TYPE {
	return LICENSE_CORRUPTED;
};

static FullLicenseInfo make_license() {
	FullLicenseInfo lic("test_license.lic", "PRODUCT", "signature");
	return lic;
}

}  // namespace

BOOST_AUTO_TEST_CASE(all_ok_is_valid) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	verifiers.push_back(ok_verifier);
	LicenseVerifier verifier(move(verifiers));
	FullLicenseInfo lic = make_license();
	LicenseInfoEx out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_EQUAL(out.return_code, FUNC_RET_OK);
}

BOOST_AUTO_TEST_CASE(one_failing_is_error) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	verifiers.push_back(fail_verifier);
	LicenseVerifier verifier(move(verifiers));
	FullLicenseInfo lic = make_license();
	LicenseInfoEx out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_ERROR);
	BOOST_CHECK_EQUAL(out.return_code, FUNC_RET_ERROR);
}

BOOST_AUTO_TEST_CASE(populates_base_fields) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	LicenseVerifier verifier(move(verifiers));
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_EXTRA_DATA] = "proprietary";
	LicenseInfoEx out;

	verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(out.license_info.license_type, LCC_LOCAL);
	BOOST_CHECK_EQUAL(string(out.license_info.proprietary_data), string("proprietary"));
}

}  // namespace test
}  // namespace license