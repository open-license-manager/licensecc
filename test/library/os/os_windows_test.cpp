#define BOOST_TEST_MODULE os_windows_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../../src/library/base/string_utils.h"
#include "../../../src/library/os/os.h"

namespace license {
namespace test {
using namespace std;

BOOST_AUTO_TEST_CASE(test_get_machine_name) {
	unsigned char identifier[6];
	FUNCTION_RETURN result = getMachineName(identifier);

	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_MESSAGE(identifier[0] != '\0', "Machine name identifier should not be empty");
}

BOOST_AUTO_TEST_CASE(test_get_disk_infos) {
	vector<DiskInfo> diskInfos;
	FUNCTION_RETURN result = getDiskInfos(diskInfos);

	// Should return OK or NOT_AVAIL (when no fixed drives are found)
	BOOST_CHECK((result == FUNC_RET_OK) || (result == FUNC_RET_NOT_AVAIL));

	// If we have disks, check that they're valid
	if (result == FUNC_RET_OK && !diskInfos.empty()) {
		BOOST_CHECK_MESSAGE(diskInfos.size() > 0, "Should have at least one disk");

		for (const auto& diskInfo : diskInfos) {
			BOOST_CHECK_MESSAGE(diskInfo.sn_initialized, "Disk serial number should be initialized");
			BOOST_CHECK_MESSAGE(diskInfo.label_initialized, "Disk label should be initialized");

			// Check that disk serial number is not all zeros
			bool allZero = true;
			for (int i = 0; i < sizeof(diskInfo.disk_sn) && allZero; i++) {
				allZero = (diskInfo.disk_sn[i] == 0);
			}
			BOOST_CHECK_MESSAGE(!allZero, "Disk serial number should not be all zeros");
		}
	}
}

BOOST_AUTO_TEST_CASE(test_get_module_name) {
	char buffer[MAX_PATH];
	FUNCTION_RETURN result = getModuleName(buffer);

	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_MESSAGE(buffer[0] != '\0', "Module name buffer should not be empty");
}

BOOST_AUTO_TEST_CASE(test_get_os_specific_identifier) {
	std::string identifier;
	FUNCTION_RETURN result = getOsSpecificIdentifier(identifier);

	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_MESSAGE(!identifier.empty(), "OS specific identifier should not be empty");
}

}  // namespace test
}  // namespace license