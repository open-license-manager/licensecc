/*
 * date_verifier.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#include <algorithm>
#include <cmath>

#include "date_verifier.hpp"
#include "../base/string_utils.h"

namespace license {
using namespace std;

LCC_EVENT_TYPE DateVerifier::verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept {
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

std::unique_ptr<LimitVerifier> DateVerifier::clone() const {
	return std::unique_ptr<LimitVerifier>(new DateVerifier());
}

} /* namespace license */