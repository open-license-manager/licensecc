/*
 * LicenseSigner.h
 *
 *  Created on: Apr 6, 2014
 *      Author: devel
 */

#ifndef LICENSESIGNER_H_
#define LICENSESIGNER_H_

#include "../library/reader/LicenseReader.h"

namespace license {

class LicenseSigner {
	const string privateKey;
	string signString(const string& license);
	string Opensslb64Encode(size_t slen, unsigned char* signature);

public:
	LicenseSigner();
	LicenseSigner(const string& alternatePrimaryKey);
	void signLicense(FullLicenseInfo& licenseInfo);
	virtual ~LicenseSigner();
};

} /* namespace license */

#endif /* LICENSESIGNER_H_ */
