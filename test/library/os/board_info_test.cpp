#define BOOST_TEST_MODULE board_info_test
#include <string>
#include <iostream>
#include <unordered_map>
#include <boost/test/unit_test.hpp>
#include "../../../src/library/os/board_info.hpp"

namespace license {
namespace test {
using namespace std;

BOOST_AUTO_TEST_CASE(board_info) {
	os::BoardInfo boardInfo;

	BOOST_CHECK_MESSAGE(boardInfo.bios_vendor().size() > 0, "Bios vendor length >0");
	BOOST_CHECK_MESSAGE(boardInfo.bios_description().size() > 0, "Bios description length >0");
	BOOST_CHECK_MESSAGE(boardInfo.sys_vendor().size() > 0, "Sys vendor length >0");
}

}  // namespace test
}  // namespace license
