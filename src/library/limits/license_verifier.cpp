/*
 * license_verifier.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include "license_verifier.hpp"
#include "../base/string_utils.h"
#include "date_verifier.hpp"
#include "pc_signature_verifier.hpp"
#include "signature_verifier.hpp"

namespace license {
using namespace std;

LicenseVerifier::LicenseVerifier() {
	m_verifiers.push_back(std::unique_ptr<LimitVerifier>(new DateVerifier()));
	m_verifiers.push_back(std::unique_ptr<LimitVerifier>(new PcSignatureVerifier()));
	m_verifiers.push_back(std::unique_ptr<LimitVerifier>(new SignatureVerifier()));
}

LicenseVerifier::LicenseVerifier(std::vector<std::unique_ptr<LimitVerifier>> verifiers)
	: m_verifiers(move(verifiers)) {}

FUNCTION_RETURN LicenseVerifier::verify_limit(const FullLicenseInfo& licInfo, EventRegistry& event_registry,
											  LicenseInfoEx& out) {
	out.license_info = LicenseInfo();
	out.license_info.license_type = LCC_LOCAL;
	out.return_code = FUNC_RET_OK;

	const auto proprietary_data = licInfo.m_limits.find(PARAM_EXTRA_DATA);
	if (proprietary_data != licInfo.m_limits.end()) {
		mstrlcpy(out.license_info.proprietary_data, proprietary_data->second.c_str(),
				 sizeof(out.license_info.proprietary_data));
	}

	bool all_ok = true;
	for (const auto& verifier : m_verifiers) {
		if (verifier == nullptr) {
			continue;
		}
		const LCC_EVENT_TYPE event = verifier->verify_limit(licInfo, out.license_info);
		if (event != LICENSE_OK) {
			event_registry.addEvent(event, licInfo.source.c_str());
		}
		all_ok = all_ok && (event < LICENSE_OK);
	}

	if (all_ok) {
		event_registry.addEvent(LICENSE_OK, licInfo.source.c_str());
	}

	const FUNCTION_RETURN overall = all_ok ? FUNC_RET_OK : FUNC_RET_ERROR;
	out.return_code = overall;
	return overall;
}

} /* namespace license */