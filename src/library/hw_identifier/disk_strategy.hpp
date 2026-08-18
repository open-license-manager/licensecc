/*
 * disk_strategy.hpp
 *
 *  Created on: Jan 14, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_DISK_STRATEGY_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_DISK_STRATEGY_HPP_

#include "identification_strategy.hpp"

namespace license {
namespace hw_identifier {

class DiskStrategy : public IdentificationStrategy {
public:
	inline DiskStrategy() {}
	virtual ~DiskStrategy();
	virtual LCC_API_HW_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual FUNCTION_RETURN alternative_ids(std::vector<HwIdentifier>& identifiers_out) const noexcept;
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_DISK_STRATEGY_HPP_ */
