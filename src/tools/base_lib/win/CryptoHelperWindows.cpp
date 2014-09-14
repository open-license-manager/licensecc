/*
 * CryptoHelperWindows.cpp
 *
 *  Created on: Sep 14, 2014
 *
 */

#include "CryptoHelperWindows.h"
// The RSA public-key key exchange algorithm
#define ENCRYPT_ALGORITHM         CALG_RSA_SIGN
// The high order WORD 0x0200 (decimal 512)
// determines the key length in bits.
#define KEYLENGTH                 0x02000000

namespace license {

CryptoHelperWindows::CryptoHelperWindows() {
	// TODO Auto-generated constructor stub

}

CryptoHelperWindows::~CryptoHelperWindows() {
	// TODO Auto-generated destructor stub
}

} /* namespace license */
