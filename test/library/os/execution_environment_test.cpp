#define BOOST_TEST_MODULE execution_environment_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../../src/library/base/string_utils.h"
#include "../../../src/library/os/network.hpp"
#include "../../../src/library/os/execution_environment.hpp"

namespace license {
namespace os {
namespace test {

using namespace license::os;
using namespace std;

// To test if virtualization is detected correctly define an env variable VIRTUAL_ENV
// otherwise the test is skipped
BOOST_AUTO_TEST_CASE(test_virtualization) {
	const char* env = getenv("VIRTUAL_ENV");
	bool docker = false;
	if (env != nullptr) {
		string required_virtualization(env);
		os::ExecutionEnvironment exec_env;
		LCC_API_VIRTUALIZATION_SUMMARY detected_virt = exec_env.virtualization();
		if (required_virtualization == "CONTAINER" || (docker = (required_virtualization == "DOCKER"))) {
			BOOST_CHECK_MESSAGE(detected_virt == LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER, "container detected");
			BOOST_CHECK_MESSAGE(exec_env.is_container(), "container detected");
			if (docker) {
				BOOST_CHECK_MESSAGE(exec_env.is_docker(), "docker detected");
			}
		} else if (required_virtualization == "VM") {
			BOOST_CHECK_MESSAGE(detected_virt == LCC_API_VIRTUALIZATION_SUMMARY::VM, "VM detected");
			BOOST_CHECK_MESSAGE(!exec_env.is_container(), "VM is not a container");
			BOOST_CHECK_MESSAGE(!exec_env.is_docker(), "VM is not a docker");
			BOOST_CHECK_MESSAGE(exec_env.virtualization_detail() != LCC_API_VIRTUALIZATION_DETAIL::BARE_TO_METAL,
								"It is not run bare to metal.");
		} else if (required_virtualization == "NONE") {
			BOOST_CHECK_EQUAL(detected_virt, LCC_API_VIRTUALIZATION_SUMMARY::NONE);
			BOOST_CHECK_MESSAGE(!exec_env.is_container(), "not a container");
			BOOST_CHECK_MESSAGE(!exec_env.is_docker(), "not a docker");
			BOOST_CHECK_MESSAGE(exec_env.virtualization_detail() == LCC_API_VIRTUALIZATION_DETAIL::BARE_TO_METAL,
								"running bare to metal.");
		} else {
			BOOST_FAIL(string("value ") + env + " not supported: VM,DOCKER,CONTAINER,NONE");
		}
	}
}

BOOST_AUTO_TEST_CASE(test_cloud_provider) {
	const char* env = getenv("CLOUD_PROVIDER");
	if (env != nullptr) {
		string required_provider(env);
		os::ExecutionEnvironment exec_env;
		LCC_API_CLOUD_PROVIDER detected_prov = exec_env.cloud_provider();
		LCC_API_VIRTUALIZATION_SUMMARY detected_virt = exec_env.virtualization();
		if (required_provider == "AZURE") {
			BOOST_CHECK_MESSAGE(detected_prov == AZURE_CLOUD, "Azure detected");
			BOOST_CHECK_MESSAGE(detected_virt != NONE, "virtualization detected");
		} else if (required_provider == "GCP") {
			BOOST_CHECK_MESSAGE(detected_prov == GOOGLE_CLOUD, "Google detected");
			BOOST_CHECK_MESSAGE(detected_virt != NONE, "virtualization detected");
		} else if (required_provider == "ON_PREMISE") {
			BOOST_CHECK_MESSAGE(detected_prov == ON_PREMISE, "On Premise detected");
		} else {
			BOOST_FAIL(string("value ") + env + " not supported: GCP,AZURE,ON_PREMISE");
		}
	}
}
}  // namespace test
}  // namespace os
}  // namespace license
