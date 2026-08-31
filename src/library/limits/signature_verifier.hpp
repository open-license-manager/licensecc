/*
 * signature_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_SIGNATURE_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_SIGNATURE_VERIFIER_HPP_

#include "limit_verifier.hpp"

namespace license {

/**
 * Verifies the cryptographic signature of the license file.
 */
class SignatureVerifier : public LimitVerifier {
public:
	SignatureVerifier() {}
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept override;
	virtual std::unique_ptr<LimitVerifier> clone() const override;
	virtual ~SignatureVerifier() {}
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_SIGNATURE_VERIFIER_HPP_ */