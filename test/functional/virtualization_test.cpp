#define BOOST_TEST_MODULE test_virtualization

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include <licensecc/licensecc.h>
#include "../../src/library/base/base.h"
#include "generate-license.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

namespace license {
namespace test {

// The license is generated and verified only if the environment variable VIRTUAL_ENV
// is defined (same convention as execution_environment_test), otherwise the tests
// are skipped. Values: NONE, VM, CONTAINER, DOCKER.
static bool required_virtualization(string& out) {
	const char* env = getenv("VIRTUAL_ENV");
	if (env == nullptr) {
		return false;
	}
	out = string(env);
	if (out == "DOCKER") {
		out = "CONTAINER";
	}
	BOOST_REQUIRE_MESSAGE(out == "NONE" || out == "VM" || out == "CONTAINER",
						  "VIRTUAL_ENV value supported: NONE, VM, CONTAINER, DOCKER. found: " + out);
	return true;
}

static string other_virtualization(const string& virt) { return (virt == "NONE") ? "VM" : "NONE"; }

BOOST_AUTO_TEST_CASE(license_virtualization_match) {
	string virt;
	if (!required_virtualization(virt)) {
		BOOST_TEST_MESSAGE("VIRTUAL_ENV not defined, test skipped");
		return;
	}
	vector<string> extraArgs;
	extraArgs.push_back("--" PARAM_VIRTUALIZATION_TYPE);
	extraArgs.push_back(virt);
	const string licLocation = generate_license("virtualization_match", extraArgs);

	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);

	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
}

BOOST_AUTO_TEST_CASE(license_virtualization_mismatch) {
	string virt;
	if (!required_virtualization(virt)) {
		BOOST_TEST_MESSAGE("VIRTUAL_ENV not defined, test skipped");
		return;
	}
	vector<string> extraArgs;
	extraArgs.push_back("--" PARAM_VIRTUALIZATION_TYPE);
	extraArgs.push_back(other_virtualization(virt));
	const string licLocation = generate_license("virtualization_mismatch", extraArgs);

	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);

	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
}

}  // namespace test
}  // namespace license
