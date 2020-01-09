/*
 * default_strategy.hpp
 *
 *  Created on: Jan 2, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_DEFAULT_STRATEGY_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_DEFAULT_STRATEGY_HPP_
#include "identification_strategy.hpp"

namespace license {

class DefaultStrategy : public IdentificationStrategy {
public:
	DefaultStrategy();
	DefaultStrategy(const DefaultStrategy &other);
	virtual ~DefaultStrategy();
	virtual IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual void identify_pc(PcIdentifier &identifier) const;
	virtual std::vector<PcIdentifier> alternative_ids() const;
	virtual EVENT_TYPE validate_identifier(const PcIdentifier &identifier) const;
};

} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_DEFAULT_STRATEGY_HPP_ */
