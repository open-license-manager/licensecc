#define BOOST_TEST_MODULE os_linux_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../src/library/base/StringUtils.h"
#include "../../src/library/os/os.h"
#include "../../src/library/os/execution_environment.hpp"

namespace license {
using namespace std;
namespace test {

BOOST_AUTO_TEST_CASE(read_disk_id) {
	ExecutionEnvironment exec_env;
	VIRTUALIZATION virt = exec_env.getVirtualization();
	if (virt == NONE || virt == VM) {
		DiskInfo *diskInfos = NULL;
		size_t disk_info_size = 0;
		FUNCTION_RETURN result = getDiskInfos(NULL, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_CHECK_GT(disk_info_size, 0);
		diskInfos = (DiskInfo *)malloc(sizeof(DiskInfo) * disk_info_size);
		result = getDiskInfos(diskInfos, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_CHECK_GT(mstrnlen_s(diskInfos[0].device, sizeof(diskInfos[0].device)), 0);
		BOOST_CHECK_GT(mstrnlen_s(diskInfos[0].label, sizeof diskInfos[0].label), 0);
		BOOST_CHECK_GT(diskInfos[0].disk_sn[0], 0);
		free(diskInfos);
	} else if (virt == CONTAINER) {
		// docker or lxc diskInfo is not meaningful
		DiskInfo *diskInfos = NULL;
		size_t disk_info_size = 0;
		FUNCTION_RETURN result = getDiskInfos(NULL, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_NOT_AVAIL);
	}
}

// To test if virtualization is detected correctly define an env variable VIRT_ENV
// otherwise the test is skipped
BOOST_AUTO_TEST_CASE(test_virtualization) {
	const char *env = getenv("VIRT_ENV");
	ExecutionEnvironment exec_env;
	if (env != NULL) {
		if (strcmp(env, "CONTAINER") == 0) {
			VIRTUALIZATION virt = exec_env.getVirtualization();
			BOOST_CHECK_MESSAGE(virt == CONTAINER, "container detected");
		} else if (strcmp(env, "VM") == 0) {
			BOOST_FAIL("check for vm not implemented");
		} else if (strcmp(env, "NONE") == 0) {
			VIRTUALIZATION virt = exec_env.getVirtualization();
			BOOST_CHECK_EQUAL(virt, NONE);
		} else {
			BOOST_FAIL(string("value ") + env + " not supported: VM,CONTAINER,NONE");
		}
	}
}
}  // namespace test
}  // namespace license
