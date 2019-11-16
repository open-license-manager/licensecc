/*
 * verifier.hpp
 *
 *  Created on: Nov 16, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_OS_VERIFIER_HPP_
#define SRC_LIBRARY_OS_VERIFIER_HPP_

#include <string>
#include "../base/base.h"

namespace license {

class Verifier {

public:
	Verifier();
	FUNCTION_RETURN verifySignature(const std::string& stringToVerify, const std::string& signatureB64);
	~Verifier();
};

} /* namespace license */

#endif /* SRC_LIBRARY_OS_VERIFIER_HPP_ */
