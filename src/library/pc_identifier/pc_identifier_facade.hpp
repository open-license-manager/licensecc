/*
 * pc_identifier_facade.hpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_
#include <string>
#include <unordered_map>
#include <bits/unique_ptr.h>
#include <licensecc/datatypes.h>
#include "pc_identifier_facade.hpp"
#include "identification_strategy.hpp"

namespace license {
namespace pc_identifier {

class PcIdentifierFacade {
private:
	PcIdentifierFacade(){};
	virtual ~PcIdentifierFacade(){};
public:
	static LCC_EVENT_TYPE validate_pc_signature(const std::string& str_code);
	static std::string generate_user_pc_signature(LCC_API_IDENTIFICATION_STRATEGY strategy);
};

}  // namespace pc_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_ */
