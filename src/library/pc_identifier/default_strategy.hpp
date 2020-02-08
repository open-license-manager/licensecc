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
namespace pc_identifier {

class DefaultStrategy : public IdentificationStrategy {
public:
	DefaultStrategy();
	virtual ~DefaultStrategy();
	virtual LCC_API_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual FUNCTION_RETURN identify_pc(PcIdentifier &pc_id) const;
	virtual std::vector<PcIdentifier> alternative_ids() const;
	virtual LCC_EVENT_TYPE validate_identifier(const PcIdentifier &identifier) const;
};
}  // namespace pc_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_DEFAULT_STRATEGY_HPP_ */
