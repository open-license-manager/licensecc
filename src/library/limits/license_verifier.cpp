/*
 * license_verifier.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include "license_verifier.hpp"
#include "../base/string_utils.h"
#include "limit_verifiers.hpp"

namespace license {
using namespace std;

LicenseVerifier::LicenseVerifier() : LicenseVerifier(std::vector<LimitVerifierFn>()) {}

LicenseVerifier::LicenseVerifier(const std::vector<LimitVerifierFn>& extra_verifiers) : m_verifiers(extra_verifiers) {
#if (ADD_DEFAULT_LIMITS == 1)
	m_verifiers.push_back(verify_date);
	m_verifiers.push_back(verify_pc_signature);
	m_verifiers.push_back(verify_virtualization);
	m_verifiers.push_back(verify_signature);
#else
	if (extra_verifiers.size() == 0) {
		LOG_ERROR("No extra_verifier specified and default verifiers not added. License verification will fail.")
	}
#endif
}

FUNCTION_RETURN LicenseVerifier::verify_limit(const FullLicenseInfo& licInfo, EventRegistry& event_registry,
											  LicenseInfo& out) {
	out = LicenseInfo();
	out.license_type = LCC_LOCAL;

	const auto proprietary_data = licInfo.m_limits.find(PARAM_EXTRA_DATA);
	if (proprietary_data != licInfo.m_limits.end()) {
		mstrlcpy(out.proprietary_data, proprietary_data->second.c_str(), sizeof(out.proprietary_data));
	}
	bool all_ok = (m_verifiers.size() != 0);
	for (const auto& verifier : m_verifiers) {
		if (!verifier) {
			continue;
		}
		const LCC_EVENT_TYPE event = verifier(licInfo, out);
		if (event != LICENSE_OK) {
			event_registry.addEvent(event, licInfo.source.c_str());
		}
		all_ok = all_ok && (event >= LICENSE_OK);
	}

	if (all_ok) {
		event_registry.addEvent(LICENSE_OK, licInfo.source.c_str());
	}

	return all_ok ? FUNC_RET_OK : FUNC_RET_ERROR;
}

} /* namespace license */