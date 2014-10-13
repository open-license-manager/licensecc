#define BOOST_TEST_MODULE os_linux_test
//#define BOOST_TEST_MAIN
//#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../../src/library/os/os.h"
#include <build_properties.h>

#include <string>
#include <iostream>
using namespace std;

BOOST_AUTO_TEST_CASE( read_disk_id ) {
	DiskInfo * diskInfos = NULL;
	size_t disk_info_size = 0;
	FUNCTION_RETURN result = getDiskInfos(NULL, &disk_info_size);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_GT(disk_info_size, 0);
	diskInfos = (DiskInfo*) malloc(sizeof(DiskInfo) * disk_info_size);
	result = getDiskInfos(diskInfos, &disk_info_size);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_GT(strlen(diskInfos[0].device), 0);
	BOOST_CHECK_GT(strlen(diskInfos[0].label), 0);
	BOOST_CHECK_GT(diskInfos[0].disk_sn[0], 0);
	free(diskInfos);
}

BOOST_AUTO_TEST_CASE( read_network_adapters ) {
	OsAdapterInfo * adapter_info = NULL;
	size_t adapter_info_size = 0;
	FUNCTION_RETURN result = getAdapterInfos(NULL, &adapter_info_size);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_GT(adapter_info_size, 0);
	adapter_info = (OsAdapterInfo*) malloc(
			sizeof(OsAdapterInfo) * adapter_info_size);
	result = getAdapterInfos(adapter_info, &adapter_info_size);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	for (size_t i = 0; i < adapter_info_size; i++) {
		cout << "Interface found: " << string(adapter_info[i].description)
				<< endl;
		BOOST_CHECK_GT(strlen(adapter_info[i].description), 0);
		//lo mac address is always 0 but it has ip
		//other interfaces may not be connected
		if (string(adapter_info[i].description) == "lo") {
			BOOST_CHECK_NE(adapter_info[i].ipv4_address[0], 0);
		} else {
			bool mac_is_0 = true;
			for(int j=0;j<6;j++){
				mac_is_0 = mac_is_0 && (adapter_info[i].mac_address[j]==0);
			}
			BOOST_CHECK_MESSAGE( !mac_is_0,
					"Mac address for interface " << adapter_info[i].description << " is 0");
		}
	}
	free(adapter_info);
}
