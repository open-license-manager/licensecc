/*
 * pc_identifier_facade.hpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_
#include <string>
#include <licensecc/datatypes.h>

namespace license {
class PcIdentifierFacade {
private:
	PcIdentifierFacade(){};
	virtual ~PcIdentifierFacade(){};
public:
	static EVENT_TYPE validate_pc_signature(const std::string& str_code);
	static std::string generate_user_pc_signature(IDENTIFICATION_STRATEGY strategy);
};

} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_FACADE_HPP_ */
