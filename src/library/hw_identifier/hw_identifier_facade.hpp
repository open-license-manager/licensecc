/*
 * hw_identifier_facade.hpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_HW_IDENTIFIER_FACADE_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_HW_IDENTIFIER_FACADE_HPP_

#include <string>
#include <licensecc/datatypes.h>
#include "identification_strategy.hpp"

namespace license {
namespace hw_identifier {

class HwIdentifierFacade {
private:
	HwIdentifierFacade() {}
	virtual ~HwIdentifierFacade() {}

public:
	static LCC_EVENT_TYPE validate_pc_signature(const std::string& str_code);
	/**
	 *
	 * @throws logic_error if pc_identifier can't be generated.
	 * @param strategy
	 * @return
	 */
	static std::string generate_user_pc_signature(LCC_API_HW_IDENTIFICATION_STRATEGY strategy);
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_HW_IDENTIFIER_FACADE_HPP_ */
