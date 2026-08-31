/*
 * signature_verifier.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include "signature_verifier.hpp"
#include "../os/signature_verifier.hpp"

namespace license {
using namespace std;

LCC_EVENT_TYPE SignatureVerifier::verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
	try {
		const string licInfoData(licInfo.printForSign());

		const FUNCTION_RETURN ret = license::os::verify_signature(licInfoData, licInfo.license_signature);

		return (ret == FUNC_RET_OK) ? SIGNATURE_VERIFIED : LICENSE_CORRUPTED;
	} catch (const std::exception&) {
		return LICENSE_CORRUPTED;
	}
}

std::unique_ptr<LimitVerifier> SignatureVerifier::clone() const {
	return std::unique_ptr<LimitVerifier>(new SignatureVerifier());
}

} /* namespace license */