/*
 * license_facade.cpp
 *
 *  Created on: Aug 1, 2026
 *      Author: Your Name
 */

#define __STDC_WANT_LIB_EXT1__ 1
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <algorithm>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include <licensecc_properties.h>

#include "base/logger.h"
#include "base/string_utils.h"
#include "hw_identifier/hw_identifier_facade.hpp"
#include "os/execution_environment.hpp"
#include "limits/license_verifier.hpp"
#include "base/string_utils.h"
#include "LicenseParser.hpp"
#include "license_facade.hpp"

using namespace std;

namespace license {

LicenseFacade::LicenseFacade() {}

LicenseFacade::~LicenseFacade() {}

LCC_EVENT_TYPE LicenseFacade::acquire_license(const CallerInformations* callerInformation,
											  const LicenseLocation* licenseLocation,
											  LicenseInfo* license_out) noexcept {
	const license::LicenseParser lp = license::LicenseParser(licenseLocation);
	vector<license::FullLicenseInfo> licenses;
	string project;
	size_t str_size;
	if (callerInformation != nullptr &&
		(str_size = license::mstrnlen_s(callerInformation->feature_name, sizeof callerInformation->feature_name)) > 0) {
		project = string(callerInformation->feature_name, str_size);
	} else {
		project = string(LCC_PROJECT_NAME);
	}
	license::EventRegistry er = lp.readLicenses(string(project), licenses);
	LCC_EVENT_TYPE result;
	if (licenses.size() > 0) {
		vector<LicenseInfoEx> all_license_results;
		license::LicenseVerifier verifier(er);
		for (auto full_lic_info_it = licenses.begin(); full_lic_info_it != licenses.end(); full_lic_info_it++) {
			if (callerInformation != nullptr) {
				full_lic_info_it->m_magic = callerInformation->magic;
			}
			LicenseInfoEx licInfoEx = verifier.verify_license(*full_lic_info_it);
			all_license_results.push_back(licInfoEx);
		}
		result = mergeLicenses(all_license_results, er, license_out);
	} else {
		er.turnWarningsIntoErrors();
		const AuditEvent* tmp = er.getLastFailure();
		if (tmp != nullptr)
			result = tmp->event_type;
		else
			result = LICENSE_FILE_NOT_FOUND;
		if (license_out != nullptr) {
			license_out->proprietary_data[0] = '\0';
			license_out->linked_to_pc = false;
			license_out->days_left = 0;
		}
	}
#ifndef NDEBUG
	const string evlog = er.to_string();
	LOG_DEBUG("License status %s", evlog.c_str());
#endif

	if (license_out != nullptr) {
		er.exportLastEvents(license_out->status, LCC_API_AUDIT_EVENT_NUM);
	}
	return result;
}

bool LicenseFacade::identify_pc(LCC_API_HW_IDENTIFICATION_STRATEGY pc_id_method, char* chbuffer, size_t* bufSize,
								ExecutionEnvironmentInfo* execution_environment_info) noexcept {
	bool result = false;
	if (*bufSize > LCC_API_PC_IDENTIFIER_SIZE && chbuffer != nullptr) {
		try {
			const string pc_id = license::hw_identifier::HwIdentifierFacade::generate_user_pc_signature(pc_id_method);
			license::mstrlcpy(chbuffer, pc_id.c_str(), *bufSize);
			result = true;
		} catch (const std::exception& ex) {
			LOG_ERROR("Error calculating hw_identifier: %s", ex.what());
#ifndef NDEBUG
			cerr << "Error occurred in identify_pc: " << ex.what() << std::endl;
#endif
		}
	} else {
		*bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	}
	static const license::os::ExecutionEnvironment exec_env;
	if (execution_environment_info != nullptr) {
		execution_environment_info->cloud_provider = exec_env.cloud_provider();
		execution_environment_info->virtualization = exec_env.virtualization();
		execution_environment_info->virtualization_detail = exec_env.virtualization_detail();
	}
	return result;
}

LCC_EVENT_TYPE LicenseFacade::mergeLicenses(const std::vector<LicenseInfoEx>& licenses, EventRegistry& er,
											LicenseInfo* license_out) noexcept {
	if (licenses.empty()) {
		if (license_out != nullptr) {
			license_out->proprietary_data[0] = '\0';
			license_out->linked_to_pc = false;
			license_out->days_left = 0;
			license_out->has_expiry = true;
		}
		er.turnWarningsIntoErrors();
		const AuditEvent* last_failure = er.getLastFailure();
		return (last_failure != nullptr) ? last_failure->event_type : LICENSE_FILE_NOT_FOUND;
	}

	LCC_EVENT_TYPE error_code;
	std::vector<LicenseInfoEx> licenses_to_process;
	bool success = false;

	for (const auto& lic_ex : licenses) {
		if (lic_ex.return_code == FUNC_RET_OK) {
			// if success process only good licenses
			licenses_to_process.push_back(lic_ex);
			success = true;
		}
	}
	if (success) {
		er.turnErrorsIntoWarnings();
		error_code = LICENSE_OK;
	} else {
		er.turnWarningsIntoErrors();
		error_code = er.getLastFailure()->event_type;
		licenses_to_process = licenses;
	}

	if (license_out != nullptr) {
		int days_left = INT_MIN;
		for (auto it = licenses.begin(); it != licenses.end(); it++) {
			// choose the license that expires later...
			if (!it->license_info.has_expiry) {
				*license_out = it->license_info;
				break;
			} else if (days_left < (int)it->license_info.days_left) {
				*license_out = it->license_info;
				days_left = it->license_info.days_left;
			}
		}
	}
	return error_code;
}

} /* namespace license */