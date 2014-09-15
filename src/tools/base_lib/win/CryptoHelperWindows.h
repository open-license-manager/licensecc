/*
 * CryptoHelperWindows.h
 *
 *  Created on: Sep 14, 2014
 *
 */

#ifndef CRYPTOHELPERWINDOWS_H_
#define CRYPTOHELPERWINDOWS_H_

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>
#include "../CryptoHelper.h"

namespace license {

class CryptoHelperWindows: public CryptoHelper {
public:
	CryptoHelperWindows();

	virtual void generateKeyPair();
	virtual const string exportPrivateKey() const;
	virtual const string exportPublicKey() const;

	virtual string signString(const void* privateKey, size_t pklen,
			const string& license) const;

	virtual ~CryptoHelperWindows();
};

} /* namespace license */

#endif /* CRYPTOHELPERWINDOWS_H_ */
