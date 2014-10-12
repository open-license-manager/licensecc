#ifndef CRYPTPHELPER_H_
#define CRYPTPHELPER_H_

#include <memory>
#include <cstddef>
#include <string>

namespace license {
using namespace std;
/**
 * Helper class definition to generate and export Public/Private keys
 * for Asymmetric encryption.
 *
 * <p>Since this part relies heavily on operating system libraries this class
 * provides a common facade to the cryptographic functions. The two implementing
 * subclasses are chosen in the factory method #getInstance(). This is to avoid
 * to clutter the code with many "ifdef". (extreme performance is not an issue here)</p>
 *<p> *it is shared by bootstrap and license-generator projects.</p>
 */

class CryptoHelper {

protected:
	inline CryptoHelper(){};

public:
	virtual void generateKeyPair() = 0;
	virtual const string exportPrivateKey() const = 0;
	virtual const string exportPublicKey() const = 0;

	virtual const string signString(const void* privateKey,
			size_t pklen, const string& license) const = 0;
	static unique_ptr<CryptoHelper> getInstance();
	inline virtual ~CryptoHelper(){};
};
}
#endif
