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

class OkVerifier : public LimitVerifier {
public:
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo&, LicenseInfo&) noexcept override { return LICENSE_OK; }
	virtual std::unique_ptr<LimitVerifier> clone() const override {
		return std::unique_ptr<LimitVerifier>(new OkVerifier());
	}
};

class FailVerifier : public LimitVerifier {
public:
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo&, LicenseInfo&) noexcept override {
		return LICENSE_CORRUPTED;
	}
	virtual std::unique_ptr<LimitVerifier> clone() const override {
		return std::unique_ptr<LimitVerifier>(new FailVerifier());
	}
};

static FullLicenseInfo make_license() {
	FullLicenseInfo lic("test_license.lic", "PRODUCT", "signature");
	return lic;
}

}  // namespace

BOOST_AUTO_TEST_CASE(all_ok_is_valid) {
	EventRegistry registry;
	std::vector<std::unique_ptr<LimitVerifier>> verifiers;
	verifiers.push_back(std::unique_ptr<LimitVerifier>(new OkVerifier()));
	verifiers.push_back(std::unique_ptr<LimitVerifier>(new OkVerifier()));
	LicenseVerifier verifier(move(verifiers));
	FullLicenseInfo lic = make_license();
	LicenseInfoEx out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_EQUAL(out.return_code, FUNC_RET_OK);
}

BOOST_AUTO_TEST_CASE(one_failing_is_error) {
	EventRegistry registry;
	std::vector<std::unique_ptr<LimitVerifier>> verifiers;
	verifiers.push_back(std::unique_ptr<LimitVerifier>(new OkVerifier()));
	verifiers.push_back(std::unique_ptr<LimitVerifier>(new FailVerifier()));
	LicenseVerifier verifier(move(verifiers));
	FullLicenseInfo lic = make_license();
	LicenseInfoEx out;

	const FUNCTION_RETURN result = verifier.verify_limit(lic, registry, out);

	BOOST_CHECK_EQUAL(result, FUNC_RET_ERROR);
	BOOST_CHECK_EQUAL(out.return_code, FUNC_RET_ERROR);
}

BOOST_AUTO_TEST_CASE(populates_base_fields) {
	EventRegistry registry;
	std::vector<std::unique_ptr<LimitVerifier>> verifiers;
	verifiers.push_back(std::unique_ptr<LimitVerifier>(new OkVerifier()));
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