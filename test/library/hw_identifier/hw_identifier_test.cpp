/*
 * Test on class HwIdentifier
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#define BOOST_TEST_MODULE test_hw_identifier

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <boost/filesystem.hpp>
#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include <licensecc/licensecc.h>
#include "../../../src/library/hw_identifier/hw_identifier.hpp"

namespace license {
namespace test {
using namespace std;
using namespace license::hw_identifier;

/**
 * Test get and set and compare hardware identifier data
 */
BOOST_AUTO_TEST_CASE(set_and_compare_data) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0xFF, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x43};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	BOOST_CHECK_MESSAGE(pc_id.data_match(data), "Data match");
}
/**
 * Test get and set and compare hardware identifier data
 */
BOOST_AUTO_TEST_CASE(compare_wrong_data) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x43};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	data[4] = 0;
	BOOST_CHECK_MESSAGE(!pc_id.data_match(data), "Data shouldn't match");
}

/**
 * Test get and set and compare hardware identifier data
 */
BOOST_AUTO_TEST_CASE(test_equals) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x43};
	HwIdentifier reference;
	reference.set_data(data);
	reference.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET);

	HwIdentifier id1;
	id1.set_data(data);
	id1.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK);
	BOOST_CHECK_MESSAGE((reference != id1), "Identification strategy different");

	HwIdentifier id2(reference);
	data[4] = 0;
	id2.set_data(data);
	BOOST_CHECK_MESSAGE((reference != id2), "data different");
}

/**
 * Print a hardware identifier and read it from the same string, check the data matches
 */
BOOST_AUTO_TEST_CASE(print_and_read) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0x42, 0x02, 0x42, 0x42, 0x42, 0x42, 0x42, 0x43};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	pc_id.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET);
	pc_id.set_is_cloud(true);
	pc_id.set_virtualization_summary(LCC_API_VIRTUALIZATION_SUMMARY::VM);
	string pc_id_str = pc_id.print();
	cout << pc_id_str << endl;
	const HwIdentifier id2(pc_id_str);
	BOOST_CHECK_MESSAGE(id2.get_identification_strategy() == LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET,
						"Strategy decoded correctly");
	BOOST_CHECK_MESSAGE(id2.get_virtualization_summary() == LCC_API_VIRTUALIZATION_SUMMARY::VM,
						"Virtualization summary decoded correctly");
	BOOST_CHECK_MESSAGE(id2.is_cloud(), "isCloud decoded correctly");
	BOOST_CHECK_MESSAGE(id2.data_match(data), "Data deserialized correctly");
	BOOST_CHECK(pc_id == id2);	// check == method
}

}  // namespace test
}  // namespace license
