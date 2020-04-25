#define BOOST_TEST_MODULE network_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../../src/library/base/StringUtils.h"
#include "../../../src/library/os/network.hpp"
#include "../../../src/library/os/execution_environment.hpp"

namespace license {
namespace os {
namespace test {

using namespace license::os;
using namespace std;

// To test if virtualization is detected correctly define an env variable VIRT_ENV
// otherwise the test is skipped
BOOST_AUTO_TEST_CASE(test_virtualization) {
	const char *env = getenv("VIRTUAL_ENV");
	os::ExecutionEnvironment exec_env;
	bool docker = false;
	if (env != nullptr) {
		LCC_API_VIRTUALIZATION_SUMMARY virt = exec_env.virtualization();
		if (strcmp(env, "CONTAINER") == 0 || (docker = (strcmp(env, "DOCKER") == 0))) {
			BOOST_CHECK_MESSAGE(virt == LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER, "container detected");
			BOOST_CHECK_MESSAGE(exec_env.is_container(), "container detected");
			if (docker) {
				BOOST_CHECK_MESSAGE(exec_env.is_docker(), "docker detected");
			}
		} else if (strcmp(env, "VM") == 0) {
			BOOST_CHECK_MESSAGE(virt == LCC_API_VIRTUALIZATION_SUMMARY::VM, "VM detected");
			BOOST_CHECK_MESSAGE(!exec_env.is_container(), "VM is not a container");
			BOOST_CHECK_MESSAGE(!exec_env.is_docker(), "VM is not a docker");
		} else if (strcmp(env, "NONE") == 0) {
			BOOST_CHECK_EQUAL(virt, LCC_API_VIRTUALIZATION_SUMMARY::NONE);
			BOOST_CHECK_MESSAGE(!exec_env.is_container(), "not a container");
			BOOST_CHECK_MESSAGE(!exec_env.is_docker(), "not a docker");
		} else {
			BOOST_FAIL(string("value ") + env + " not supported: VM,DOCKER,CONTAINER,NONE");
		}
	}
}
}  // namespace test
}  // namespace os
}  // namespace license
