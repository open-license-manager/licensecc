#define BOOST_TEST_MODULE "test_license_reader"
#define __STDC_WANT_LIB_EXT1__ 1

#include <string>

#include <boost/test/unit_test.hpp>
#include <vector>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include <licensecc/datatypes.h>

#include "../../src/library/base/EventRegistry.h"
#include "../../src/library/base/file_utils.hpp"
#include "../../src/library/os/os.h"
#include "../../src/library/locate/LocatorFactory.hpp"
#include "../../src/library/locate/FoundLicenseCursor.hpp"
#include "../../src/library/LicenseParser.hpp"
namespace license {
namespace test {

using namespace license;
using namespace std;

/**
 * Read license at application provided location
 */
BOOST_AUTO_TEST_CASE(read_single_file) {
	string location = PROJECT_TEST_SRC_DIR "/library/test_reader.ini";

	EventRegistry registry;
	registry.addEvent(LCC_EVENT_TYPE::LICENSE_FOUND, location);
	LicenseParser licenseParser(registry);
	string license = get_file_contents(location.c_str(), LCC_API_MAX_LICENSE_DATA_LENGTH);
	cout << license << endl;
	locate::RawLicenseData rawLicense(location, license);
	// the product name is always converted to uppercase (case insensitive.)
	const vector<FullLicenseInfo> licenseInfos = licenseParser.parseLicense("PrODUCT", rawLicense);
	BOOST_CHECK(registry.isGood());
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
}

/**
 * Test the error return if the product code is not found in the license
 */
BOOST_AUTO_TEST_CASE(product_not_licensed) {
	string location = PROJECT_TEST_SRC_DIR "/library/test_reader.ini";

	EventRegistry registry;
	registry.addEvent(LCC_EVENT_TYPE::LICENSE_FOUND, location);
	LicenseParser licenseParser(registry);
	locate::RawLicenseData rawLicense(location, get_file_contents(location.c_str(), LCC_API_MAX_LICENSE_DATA_LENGTH));
	const vector<FullLicenseInfo> licenseInfos = licenseParser.parseLicense("NOT-A-PRODUCT", rawLicense);
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	registry.turnWarningsIntoErrors();
	BOOST_CHECK(!registry.isGood());
	BOOST_ASSERT(registry.getLastFailure() != NULL);
	BOOST_CHECK_EQUAL(PRODUCT_NOT_LICENSED, registry.getLastFailure()->event_type);
}

}  // namespace test
}  // namespace license
