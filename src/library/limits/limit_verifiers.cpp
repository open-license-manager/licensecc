/*
 * limit_verifiers.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include <algorithm>
#include <cmath>
#include <sstream>

#include "limit_verifiers.hpp"
#include "../base/base.h"
#include "../base/string_utils.h"
#include "../base/logger.h"
#include "../hw_identifier/hw_identifier_facade.hpp"
#include "../os/signature_verifier.hpp"

namespace license {
using namespace std;

string printForSign(const FullLicenseInfo& licInfo) {
	ostringstream oss;
	oss << toupper_copy(trim_copy(licInfo.m_project));
	for (auto& it : licInfo.m_limits) {
		if (it.first != LICENSE_SIGNATURE) {
			oss << trim_copy(it.first) << trim_copy(it.second);
		}
	}

	LOG_DEBUG("license to sign [%s]", oss.str().c_str());
	return oss.str();
}

LCC_EVENT_TYPE verify_date(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
	try {
		bool is_valid = true;
		const time_t now = time(nullptr);

		const auto expiry = licInfo.m_limits.find(PARAM_EXPIRY_DATE);
		if (expiry != licInfo.m_limits.end()) {
			mstrlcpy(out.expiry_date, expiry->second.c_str(), sizeof(out.expiry_date));
			out.has_expiry = true;
			time_t expiry_seconds;
			if (seconds_from_epoch(expiry->second, expiry_seconds)) {
				if (expiry_seconds < now) {
					is_valid = false;
				}
				const double secs = difftime(expiry_seconds, now);
				out.days_left = max((int)round(secs / (60 * 60 * 24)), 0);
			} else {
				out.days_left = 0;
				is_valid = false;
			}
		} else {
			out.has_expiry = false;
			out.days_left = 9999;
		}

		const auto start_date = licInfo.m_limits.find(PARAM_BEGIN_DATE);
		if (start_date != licInfo.m_limits.end()) {
			time_t start_seconds;
			if (!seconds_from_epoch(start_date->second, start_seconds) || start_seconds > now) {
				is_valid = false;
			}
		}

		return is_valid ? LICENSE_OK : PRODUCT_EXPIRED;
	} catch (const std::exception&) {
		return LICENSE_CORRUPTED;
	}
}

LCC_EVENT_TYPE verify_pc_signature(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
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

LCC_EVENT_TYPE verify_signature(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
	try {
		const string licInfoData(printForSign(licInfo));

		const FUNCTION_RETURN ret = license::os::verify_signature(licInfoData, licInfo.license_signature);

		return (ret == FUNC_RET_OK) ? SIGNATURE_VERIFIED : LICENSE_CORRUPTED;
	} catch (const std::exception&) {
		return LICENSE_CORRUPTED;
	}
}

} /* namespace license */