#define BOOST_TEST_MODULE os_linux_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../src/library/base/string_utils.h"
#include "../../src/library/os/os.h"
#include "../../src/library/os/execution_environment.hpp"

namespace license {
namespace test {
using namespace std;
using namespace os;

BOOST_AUTO_TEST_CASE(read_disk_id) {
	os::ExecutionEnvironment exec_env;
	LCC_API_VIRTUALIZATION_SUMMARY virt = exec_env.virtualization();
	if (virt == LCC_API_VIRTUALIZATION_SUMMARY::NONE || virt == LCC_API_VIRTUALIZATION_SUMMARY::VM) {
		DiskInfo *diskInfos = NULL;
		size_t disk_info_size = 0;
		FUNCTION_RETURN result = getDiskInfos(NULL, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_CHECK_GT(disk_info_size, 0);
		diskInfos = (DiskInfo*) malloc(sizeof(DiskInfo) * disk_info_size);
		result = getDiskInfos(diskInfos, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_CHECK_GT(mstrnlen_s(diskInfos[0].device, sizeof(diskInfos[0].device)), 0);
		BOOST_CHECK_GT(mstrnlen_s(diskInfos[0].label, sizeof diskInfos[0].label), 0);
		bool all_zero = true;
		for (int i = 0; i < sizeof(diskInfos[0].disk_sn) && all_zero; i++) {
			all_zero = (diskInfos[0].disk_sn[i] != 0);
		}
		BOOST_CHECK_MESSAGE(!all_zero, "disksn is not all zero");
		free(diskInfos);
	} else if (virt == LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER) {
		// docker or lxc diskInfo is not meaningful
		DiskInfo *diskInfos = NULL;
		size_t disk_info_size = 0;
		FUNCTION_RETURN result = getDiskInfos(NULL, &disk_info_size);
		BOOST_CHECK_EQUAL(result, FUNC_RET_NOT_AVAIL);
	}
}

} // namespace test
} // namespace license
