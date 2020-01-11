/*
 * identification_strategy.hpp
 *
 *  Created on: Jan 1, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_

#include <licensecc/datatypes.h>
#include <licensecc_properties.h>

#include <vector>
#include "../base/base.h"
#include "pc_identifier.hpp"
namespace license {

class IdentificationStrategy {
protected:
	LCC_EVENT_TYPE validate_identifier(const PcIdentifier& identifier,
									   const std::vector<std::array<uint8_t, 6>>& available_ids) const;

public:
	IdentificationStrategy(){};
	virtual ~IdentificationStrategy(){};
	virtual LCC_API_IDENTIFICATION_STRATEGY identification_strategy() const = 0;
	virtual FUNCTION_RETURN identify_pc(PcIdentifier& identifier) const = 0;
	virtual std::vector<PcIdentifier> alternative_ids() const = 0;
	virtual LCC_EVENT_TYPE validate_identifier(const PcIdentifier &identifier) const = 0;
};

} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_IDENTIFICATION_STRATEGY_HPP_ */
