/*
 * LicenseSigner.cpp (Windows)
 *
 *  Created on: Apr 6, 2014
 *      Author: devel
 */

#include "../LicenseSigner.h"
#include "../private-key.h"
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <cmath>

namespace license {
using namespace std;

LicenseSigner::LicenseSigner() {
	os_initialize();
}

LicenseSigner::LicenseSigner(const std::string& alternatePrimaryKey) {
	os_initialize();
}

string LicenseSigner::Opensslb64Encode(size_t slen, unsigned char* signature) {

	return NULL;
}

string LicenseSigner::signString(const string& license) {
	return NULL;
}

void LicenseSigner::signLicense(FullLicenseInfo& licenseInfo) {
	string license = licenseInfo.printForSign();
	string signature = signString(license);
	licenseInfo.license_signature = signature;
}

LicenseSigner::~LicenseSigner() {

}

} /* namespace license */
