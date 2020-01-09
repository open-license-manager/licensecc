/*
 * identification_strategy.hpp
 *
 *  Created on: Jan 1, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_

#include <licensecc/datatypes.h>
#include <vector>
#include "pc_identifier.hpp"
namespace license {

class IdentificationStrategy {
public:
	IdentificationStrategy(){};
	virtual ~IdentificationStrategy(){};
	virtual IDENTIFICATION_STRATEGY identification_strategy() const = 0;
	virtual void identify_pc(PcIdentifier &identifier) const = 0;
	virtual std::vector<PcIdentifier> alternative_ids() const = 0;
	virtual EVENT_TYPE validate_identifier(const PcIdentifier &identifier) const = 0;
};

} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_ */
