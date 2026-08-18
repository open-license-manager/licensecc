/*
 * system_id_strategy_test.cpp
 *
 *  Created on: Aug 13, 2026
 *      Author: Gabriele Contini
 */

#define BOOST_TEST_MODULE test_system_id_strategy

#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../../src/library/hw_identifier/system_id_strategy.hpp"
#include "../../../src/library/hw_identifier/hw_identifier.hpp"

namespace license {
namespace test {

using namespace license::hw_identifier;
BOOST_AUTO_TEST_CASE(returnsOneNonEmptyIdentifier) {
	SystemIdStrategy strategy;
	std::vector<HwIdentifier> identifiers;
	FUNCTION_RETURN ret = strategy.alternative_ids(identifiers);
	BOOST_CHECK_EQUAL(ret, FUNC_RET_OK);
	BOOST_CHECK_EQUAL(identifiers.size(), 1);
	const HwIdentifier& identifier = identifiers.at(0);
	std::string serialized = identifier.print();
	BOOST_CHECK(!serialized.empty());
}

}  // namespace test
}  // namespace license