/*
 * LicenseVerifier.cpp
 *
 *  Created on: Nov 17, 2019
 *      Author: GC
 */
#include <cmath>
#include <algorithm>

#include "license_verifier.hpp"
#include "../os/signature_verifier.h"
#include "../base/StringUtils.h"
#include "../pc-identifiers.h"

namespace license {
using namespace std;

LicenseVerifier::LicenseVerifier(EventRegistry& er) : m_event_registry(er) {}

LicenseVerifier::~LicenseVerifier() {}

FUNCTION_RETURN LicenseVerifier::verify_signature(const FullLicenseInfo& licInfo) {
	const string licInfoData(licInfo.printForSign());

	FUNCTION_RETURN ret = license::verify_signature(licInfoData, licInfo.license_signature);

	if (ret == FUNC_RET_OK) {
		m_event_registry.addEvent(SIGNATURE_VERIFIED, licInfo.source);
	} else {
		m_event_registry.addEvent(LICENSE_CORRUPTED, licInfo.source);
	}
	return ret;
}

// TODO: split in different classes
FUNCTION_RETURN LicenseVerifier::verify_limits(const FullLicenseInfo& licInfo) {
	bool is_valid = true;
	const time_t now = time(nullptr);
	auto expiry = licInfo.m_limits.find(PARAM_EXPIRY_DATE);
	if (expiry != licInfo.m_limits.end()) {
		if (seconds_from_epoch(expiry->second.c_str()) < now) {
			/*
						eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),
								string("Expired on: " + this->to_date).c_str());*/
			m_event_registry.addEvent(PRODUCT_EXPIRED, licInfo.source.c_str(), ("Expired " + expiry->second).c_str());
			is_valid = false;
		}
	}
	auto start_date = licInfo.m_limits.find(PARAM_BEGIN_DATE);
	if (is_valid && start_date != licInfo.m_limits.end()) {
		if (seconds_from_epoch(start_date->second.c_str()) > now) {
			/*eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),
					string("Valid from " + this->from_date).c_str());*/
			m_event_registry.addEvent(PRODUCT_EXPIRED, licInfo.source.c_str(),
									  ("Valid from " + start_date->second).c_str());
			is_valid = false;
		}
	}
	auto client_sig = licInfo.m_limits.find(PARAM_CLIENT_SIGNATURE);
	if (is_valid && client_sig != licInfo.m_limits.end()) {
		PcSignature str_code;
		strncpy(str_code, client_sig->second.c_str(), sizeof(str_code) - 1);
		const EVENT_TYPE event = validate_pc_signature(str_code);
		m_event_registry.addEvent(event, licInfo.source);
		is_valid = is_valid && (event == LICENSE_OK);
	}
	return is_valid ? FUNC_RET_OK : FUNC_RET_ERROR;
}

LicenseInfo LicenseVerifier::toLicenseInfo(const FullLicenseInfo& fullLicInfo) const {
	LicenseInfo info;
	info.license_type = LOCAL;

	auto expiry = fullLicInfo.m_limits.find(PARAM_EXPIRY_DATE);
	if (expiry != fullLicInfo.m_limits.end()) {
		strncpy(info.expiry_date, expiry->second.c_str(), sizeof(info.expiry_date));
		info.has_expiry = true;
		const double secs = difftime(seconds_from_epoch(expiry->second.c_str()), time(nullptr));
		info.days_left = max((int)round(secs / (60 * 60 * 24)), 0);
	} else {
		info.has_expiry = false;
		info.days_left = 9999;
		info.expiry_date[0] = '\0';
	}

	auto start_date = fullLicInfo.m_limits.find(PARAM_BEGIN_DATE);
	if (start_date != fullLicInfo.m_limits.end()) {
	}

	auto client_sig = fullLicInfo.m_limits.find(PARAM_CLIENT_SIGNATURE);
	info.linked_to_pc = (client_sig != fullLicInfo.m_limits.end());

	auto proprietary_data = fullLicInfo.m_limits.find(PARAM_EXTRA_DATA);
	if (proprietary_data != fullLicInfo.m_limits.end()) {
		strncpy(info.proprietary_data, proprietary_data->second.c_str(), PROPRIETARY_DATA_SIZE);
	}
	return info;
}

} /* namespace license */
