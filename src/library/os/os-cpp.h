/*
 * os-dependent.hpp
 *
 *  Created on: Mar 29, 2014
 *      
 */

#ifndef OS_DEPENDENT_CPP_
#define OS_DEPENDENT_CPP_

/* deprecated: all os dependent functions have been implemented in C*/
#include "os.h"

namespace license {
using namespace std;



class OsFunctions {
public:

	//static string getModuleName();
	//use system dependent methods to verify signature
	static bool verifySignature(const char * stringToVerify,
			const char* signature);

};

}

#endif /* OS_DEPENDENT_HPP_ */
