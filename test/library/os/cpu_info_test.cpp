#define BOOST_TEST_MODULE cpu_info_test
#include <string>
#include <iostream>
#include <unordered_map>
#include <boost/test/unit_test.hpp>
#include <cstdlib>

#include "../../../src/library/os/cpu_info.hpp"

namespace license {
namespace test {
using namespace std;

BOOST_AUTO_TEST_CASE(cpu_info) {
	os::CpuInfo cpuInfo;
	const char* env = getenv("VIRTUAL_ENV");
	if (env != nullptr && string(env) == "VM") {
		if (cpuInfo.virt_info_available()) {
			BOOST_CHECK_MESSAGE(cpuInfo.is_virtual(), "Hypervisor bit set when running in a VM");
		} else {
			BOOST_CHECK_MESSAGE(true, "cpu virt bit not available (arm)");
		}
	}
	BOOST_CHECK_MESSAGE(!cpuInfo.brand().empty(), "some cpu brand was returned");
	BOOST_TEST_MESSAGE(string("Vendor: ") + cpuInfo.vendor() + ",brand:" + cpuInfo.brand());
}

}  // namespace test
}  // namespace license
