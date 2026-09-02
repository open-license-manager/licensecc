#define BOOST_TEST_MODULE test_license_verifier

#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc/datatypes.h>

#include <licensecc/EventRegistry.h>
#include "../../../src/library/LicenseParser.hpp"
#include "../../../src/library/limits/license_verifier.hpp"
#include <licensecc/datatypes_cpp.hpp>

namespace license {
namespace test {

using namespace std;

namespace {

const LimitVerifierFn ok_verifier = [](const FullLicenseInfo&, LicenseInfo&) -> LCC_EVENT_TYPE { return LICENSE_OK; };

const LimitVerifierFn fail_verifier = [](const FullLicenseInfo&, LicenseInfo&) -> LCC_EVENT_TYPE {
	return IDENTIFIERS_MISMATCH;
};

static FullLicenseInfo make_license() {
	FullLicenseInfo lic;
	lic.source = "test_license.lic";
	lic.m_project = "PRODUCT";
	lic.license_signature = "signature";
	return lic;
}

}  // namespace

BOOST_AUTO_TEST_CASE(signature_verifier_fails_on_invalid_signature) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	verifiers.push_back(ok_verifier);
	LicenseVerifier verifier(verifiers);
	FullLicenseInfo lic = make_license();
	LicenseInfo out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_ERROR);

	BOOST_CHECK(registry.turnWarningsIntoErrors());
	const AuditEvent* failure = registry.getLastFailure();
	BOOST_REQUIRE(failure != nullptr);
	BOOST_CHECK_EQUAL(failure->event_type, LICENSE_CORRUPTED);
}

BOOST_AUTO_TEST_CASE(one_failing_is_error) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	verifiers.push_back(fail_verifier);
	LicenseVerifier verifier(verifiers);
	FullLicenseInfo lic = make_license();
	LicenseInfo out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_ERROR);

	BOOST_CHECK(registry.turnWarningsIntoErrors());
	AuditEvent events[LCC_API_AUDIT_EVENT_NUM];
	registry.exportLastEvents(events, LCC_API_AUDIT_EVENT_NUM);

	bool has_identifiers_mismatch = false;
	bool has_license_corrupted = false;
	for (const AuditEvent& ev : events) {
		has_identifiers_mismatch = has_identifiers_mismatch || (ev.event_type == IDENTIFIERS_MISMATCH);
		has_license_corrupted = has_license_corrupted || (ev.event_type == LICENSE_CORRUPTED);
	}
	BOOST_CHECK(has_identifiers_mismatch);
	BOOST_CHECK(has_license_corrupted);
}

BOOST_AUTO_TEST_CASE(populates_base_fields) {
	EventRegistry registry;
	std::vector<LimitVerifierFn> verifiers;
	verifiers.push_back(ok_verifier);
	LicenseVerifier verifier(verifiers);
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_EXTRA_DATA] = "proprietary";
	LicenseInfo out;

	verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(out.license_type, LCC_LOCAL);
	BOOST_CHECK_EQUAL(string(out.proprietary_data), string("proprietary"));
}

}  // namespace test
}  // namespace license