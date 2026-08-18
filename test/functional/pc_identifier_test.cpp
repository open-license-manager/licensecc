#define BOOST_TEST_MODULE integration_test_pc_identifier

#include <boost/test/unit_test.hpp>
#include <cstring>
#include <iostream>
#include <licensecc/licensecc.h>

namespace license {
namespace test {

/**
 * Check that the pc identifier can be computed through the C API entry point
 * identify_pc, returning true and a non-empty identifier.
 */
BOOST_AUTO_TEST_CASE(pc_identifier_default_strategy) {
	size_t pc_id_sz = LCC_API_PC_IDENTIFIER_SIZE;
	char pc_identifier[LCC_API_PC_IDENTIFIER_SIZE];

	const bool result = identify_pc(STRATEGY_DEFAULT, pc_identifier, &pc_id_sz, nullptr);

	BOOST_CHECK_EQUAL(result, true);
	BOOST_CHECK_GT(pc_id_sz, 0u);
	BOOST_CHECK_EQUAL(std::string(pc_identifier).length(), LCC_API_PC_IDENTIFIER_SIZE - 1);
}

}  // namespace test
}  // namespace license
