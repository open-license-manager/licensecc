#define BOOST_TEST_MODULE test_date_verifier

#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc/datatypes.h>

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

BOOST_AUTO_TEST_CASE(no_dates_is_valid) {
	FullLicenseInfo lic = make_license();
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_date(lic, out);

	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(out.has_expiry, false);
	BOOST_CHECK_EQUAL(out.days_left, (unsigned int)9999);
}

BOOST_AUTO_TEST_CASE(not_expired_is_valid) {
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_EXPIRY_DATE] = "2050-10-10";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_date(lic, out);

	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(out.has_expiry, true);
	BOOST_CHECK_GT(out.days_left, (unsigned int)0);
	BOOST_CHECK_EQUAL(string(out.expiry_date), string("2050-10-10"));
}

BOOST_AUTO_TEST_CASE(expired_is_error) {
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_EXPIRY_DATE] = "2013-10-10";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_date(lic, out);

	BOOST_CHECK_EQUAL(result, PRODUCT_EXPIRED);
	BOOST_CHECK_EQUAL(out.days_left, (unsigned int)0);
}

BOOST_AUTO_TEST_CASE(future_start_date_is_error) {
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_BEGIN_DATE] = "2050-10-10";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_date(lic, out);

	BOOST_CHECK_EQUAL(result, PRODUCT_EXPIRED);
}

BOOST_AUTO_TEST_CASE(malformed_expiry_is_error) {
	FullLicenseInfo lic = make_license();
	lic.m_limits[PARAM_EXPIRY_DATE] = "not-a-date";
	LicenseInfo out;

	const LCC_EVENT_TYPE result = verify_date(lic, out);

	BOOST_CHECK_EQUAL(result, PRODUCT_EXPIRED);
}

}  // namespace test
}  // namespace license