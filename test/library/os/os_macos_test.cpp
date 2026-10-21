#define BOOST_TEST_MODULE os_macos_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../../src/library/base/string_utils.h"
#include "../../../src/library/os/os.h"
#include "../../../src/library/os/execution_environment.hpp"

namespace license {
namespace test {
using namespace std;

BOOST_AUTO_TEST_CASE(read_disk_id) {
	os::ExecutionEnvironment exec_env;
	LCC_API_VIRTUALIZATION_SUMMARY virt = exec_env.virtualization();
	vector<DiskInfo> disk_infos;
	FUNCTION_RETURN result = getDiskInfos(disk_infos);
	if (virt == LCC_API_VIRTUALIZATION_SUMMARY::NONE || virt == LCC_API_VIRTUALIZATION_SUMMARY::VM) {
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_REQUIRE_MESSAGE(disk_infos.size() > 0, "Found some disk");
		bool uuid_found = false;
		bool id_found = false;
		bool preferred_found = false;

		for (auto disk_info : disk_infos) {
			uuid_found = uuid_found || disk_info.uuid_initialized;
			id_found = id_found || disk_info.disk_phys_id_initialized;
			preferred_found = preferred_found || disk_info.preferred;

			if (disk_info.uuid_initialized) {
				BOOST_CHECK_MESSAGE(!disk_info.disk_uuid.empty(), "disk uuid is not empty");
			}
			if (disk_info.disk_phys_id_initialized) {
				BOOST_CHECK_MESSAGE(!disk_info.disk_phys_id.empty(), "disk physical id is not empty");
			}
			if (disk_info.preferred) {
				BOOST_CHECK_MESSAGE(disk_info.uuid_initialized || disk_info.disk_phys_id_initialized,
									"serial number found on preferred disk");
			}
		}
		BOOST_CHECK_MESSAGE(uuid_found, "At least one UUID initialized");
		BOOST_CHECK_MESSAGE(id_found, "At least one physical id found");
		BOOST_CHECK_MESSAGE(preferred_found, "Root disk marked as preferred");
	} else {
		BOOST_CHECK_EQUAL(result, FUNC_RET_NOT_AVAIL);
		BOOST_REQUIRE_MESSAGE(disk_infos.size() == 0, "Found no disk");
	}
}

BOOST_AUTO_TEST_CASE(test_os_specific_id) {
	std::string identifier;
	FUNCTION_RETURN result = getOsSpecificIdentifier(identifier);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);

	// Check that the string is not empty
	BOOST_CHECK_MESSAGE(!identifier.empty(), "OS specific identifier should not be empty");
	// On macOS the identifier is the IOPlatformUUID, formatted as "8-4-4-4-12" hex groups
	BOOST_CHECK_EQUAL(identifier.size(), 36);
	int dashes = 0;
	for (char c : identifier) {
		dashes += (c == '-') ? 1 : 0;
	}
	BOOST_CHECK_EQUAL(dashes, 4);
}

BOOST_AUTO_TEST_CASE(test_module_name) {
	char buffer[MAX_PATH] = {0};
	FUNCTION_RETURN result = getModuleName(buffer);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_MESSAGE(buffer[0] == '/', "executable path is absolute");
}

BOOST_AUTO_TEST_CASE(test_machine_name) {
	unsigned char identifier[6];
	FUNCTION_RETURN result = getMachineName(identifier);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
}

}  // namespace test
}  // namespace license
