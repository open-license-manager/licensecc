/*
 * pc_signature_verifier.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include "pc_signature_verifier.hpp"
#include "../hw_identifier/hw_identifier_facade.hpp"

namespace license {
using namespace std;

LCC_EVENT_TYPE PcSignatureVerifier::verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
	try {
		const auto client_sig = licInfo.m_limits.find(PARAM_CLIENT_SIGNATURE);
		out.linked_to_pc = (client_sig != licInfo.m_limits.end());
		if (client_sig == licInfo.m_limits.end()) {
			return LICENSE_OK;
		}
		return hw_identifier::HwIdentifierFacade::validate_pc_signature(client_sig->second);
	} catch (const std::exception&) {
		return IDENTIFIER_NOT_AVAILABLE;
	}
}

std::unique_ptr<LimitVerifier> PcSignatureVerifier::clone() const {
	return std::unique_ptr<LimitVerifier>(new PcSignatureVerifier());
}

} /* namespace license */