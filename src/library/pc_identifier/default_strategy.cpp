/*
 * default_strategy.cpp
 *
 *  Created on: Jan 2, 2020
 *      Author: devel
 */

#include "default_strategy.hpp"

namespace license {

DefaultStrategy::DefaultStrategy() {
	// TODO Auto-generated constructor stub
}

DefaultStrategy::~DefaultStrategy() {
	// TODO Auto-generated destructor stub
}

DefaultStrategy::DefaultStrategy(const DefaultStrategy& other) {
	// TODO Auto-generated constructor stub
}

IDENTIFICATION_STRATEGY DefaultStrategy::identification_strategy() const { return STRATEGY_DEFAULT; }

void DefaultStrategy::identify_pc(PcIdentifier& identifier) const {}

std::vector<PcIdentifier> DefaultStrategy::alternative_ids() const {}

EVENT_TYPE DefaultStrategy::validate_identifier(const PcIdentifier& identifier) const {
	// default strategy should always realize itself as a concrete strategy
	return IDENTIFIERS_MISMATCH;
}

} /* namespace license */
