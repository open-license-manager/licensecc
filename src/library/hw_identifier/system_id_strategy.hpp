/*
 * system_id_strategy.hpp
 *
 *  Created on: Aug 13, 2026
 *      Author: Gabriele Contini
 */

#ifndef SRC_LIBRARY_HW_IDENTIFIER_SYSTEM_ID_STRATEGY_HPP_
#define SRC_LIBRARY_HW_IDENTIFIER_SYSTEM_ID_STRATEGY_HPP_
#include "identification_strategy.hpp"

namespace license {
namespace hw_identifier {

class SystemIdStrategy : public IdentificationStrategy {
public:
	SystemIdStrategy();
	virtual ~SystemIdStrategy();
	virtual LCC_API_HW_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual std::vector<HwIdentifier> alternative_ids() const;
};
}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_HW_IDENTIFIER_SYSTEM_ID_STRATEGY_HPP_ */