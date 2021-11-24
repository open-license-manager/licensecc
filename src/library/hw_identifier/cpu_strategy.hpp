/*
 * cpu_strategy.hpp
 *
 *  Created on: Nov 23, 2021
 *      Author: Tibalt
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_CPU_STRATEGY_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_CPU_STRATEGY_HPP_

#include "identification_strategy.hpp"

namespace license {
namespace hw_identifier {

class CPUStrategy : public IdentificationStrategy {
public:
	inline CPUStrategy(){};
	virtual ~CPUStrategy();
	virtual LCC_API_HW_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual std::vector<HwIdentifier> alternative_ids() const;
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_CPU_STRATEGY_HPP_*/
