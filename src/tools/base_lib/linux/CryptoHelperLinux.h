/*
 * CryptpHelperLinux.h
 *
 *  Created on: Sep 14, 2014
 *      
 */

#ifndef CRYPTPHELPERLINUX_H_
#define CRYPTPHELPERLINUX_H_
#include <openssl/rsa.h>
#include <cstddef>
#include <string>

namespace license {
using namespace std;

class CryptoHelperLinux {
private:
	static const int kBits = 1024;
	static const int kExp = 65537;
	RSA * rsa;
	const string Opensslb64Encode(size_t slen, unsigned char* signature) const;
public:
	CryptoHelperLinux();

	virtual void generateKeyPair();
	virtual const string exportPrivateKey() const;
	virtual const string exportPublicKey() const;

	virtual string signString(const void* privateKey, size_t pklen,
			const string& license) const;
	virtual ~CryptoHelperLinux();
};

} /* namespace license */

#endif /* CRYPTPHELPERLINUX_H_ */
