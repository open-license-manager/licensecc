/*
 * pc_signature_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_PC_SIGNATURE_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_PC_SIGNATURE_VERIFIER_HPP_

#include "limit_verifier.hpp"

namespace license {

/**
 * Verifies the hardware (client) signature of the license against the
 * signature generated from the current machine.
 */
class PcSignatureVerifier : public LimitVerifier {
public:
	PcSignatureVerifier() {}
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept override;
	virtual std::unique_ptr<LimitVerifier> clone() const override;
	virtual ~PcSignatureVerifier() {}
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_PC_SIGNATURE_VERIFIER_HPP_ */