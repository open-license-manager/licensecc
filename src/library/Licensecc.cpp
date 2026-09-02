/*
 * Licensecc.cpp
 *
 *  Created on: Aug 1, 2026
 *      Author: Gabriele Contini
 */

#define __STDC_WANT_LIB_EXT1__ 1
#include <fstream>
#include <cstring>
#include <cstdlib>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include <licensecc/Licensecc.hpp>
#include <licensecc_properties.h>

#include "base/logger.h"
#include "base/string_utils.h"
#include "hw_identifier/hw_identifier_facade.hpp"
#include "os/execution_environment.hpp"
#include "limits/license_verifier.hpp"
#include "base/string_utils.h"
#include "LicenseParser.hpp"
#include "locate/LocatorFactory.hpp"
#include "locate/FoundLicenseCursor.hpp"

using namespace std;

namespace license {

struct LicenseInfoEx {
	LicenseInfo license_info;
	FUNCTION_RETURN return_code;
};

Licensecc::Licensecc(const std::vector<std::unique_ptr<locate::LocatorStrategy>>* strategies_in,
					 const std::vector<LimitVerifierFn>& extra_verifiers)
	: m_strategies(strategies_in), m_verifier(new LicenseVerifier(extra_verifiers)) {}

Licensecc::~Licensecc() {}

FUNCTION_RETURN getLocatorStrategies(std::vector<std::unique_ptr<locate::LocatorStrategy>>& strategiesOut,
									 const LicenseLocation* locationHint, EventRegistry& eventRegistryRef,
									 const std::vector<std::unique_ptr<locate::LocatorStrategy>>* strategies_in) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	if (strategies_in == nullptr) {
		result = locate::LocatorFactory::get_active_strategies(strategiesOut, locationHint);
	} else {
		if (strategies_in->size() > 0) {
			for (const auto& strategy : *strategies_in) {
				strategiesOut.push_back(strategy->clone());
			}
			result = FUNC_RET_OK;
		}
	}
	return result;
}

static LCC_EVENT_TYPE no_license_found(EventRegistry& er, LicenseInfo* license_out) noexcept {
	if (license_out != nullptr) {
		license_out->proprietary_data[0] = '\0';
		license_out->linked_to_pc = false;
		license_out->days_left = 0;
		license_out->has_expiry = false;
	}
	er.turnWarningsIntoErrors();
	const AuditEvent* last_failure = er.getLastFailure();
	return (last_failure != nullptr) ? last_failure->event_type : LICENSE_FILE_NOT_FOUND;
}

static LCC_EVENT_TYPE merge_licenses(const std::vector<LicenseInfoEx>& licenses, EventRegistry& er,
									 LicenseInfo* license_out) noexcept;

LCC_EVENT_TYPE Licensecc::acquire_license(const CallerInformations* callerInformation,
										  const LicenseLocation* licenseLocation, LicenseInfo* license_out) noexcept {
	string project;
	size_t str_size;
	if (callerInformation != nullptr &&
		(str_size = license::mstrnlen_s(callerInformation->feature_name, sizeof callerInformation->feature_name)) > 0) {
		project = string(callerInformation->feature_name, str_size);
	} else {
		project = string(LCC_PROJECT_NAME);
	}
	EventRegistry er;
	LCC_EVENT_TYPE result = LICENSE_FILE_NOT_FOUND;
	std::vector<std::unique_ptr<locate::LocatorStrategy>> strategies;
	const FUNCTION_RETURN strategiesRet = getLocatorStrategies(strategies, licenseLocation, er, m_strategies);
	if (strategiesRet == FUNC_RET_OK && strategies.size() > 0) {
		const license::LicenseParser lp = license::LicenseParser(er);
		locate::FoundLicenseCursor cursor(strategies, er);
		vector<LicenseInfoEx> all_license_results;
		for (auto it : cursor) {
			const locate::RawLicenseData rawLicense = it;
			vector<license::FullLicenseInfo> parsed = lp.parseLicense(project, rawLicense);
			for (auto& licInfo : parsed) {
				if (callerInformation != nullptr) {
					licInfo.m_magic = callerInformation->magic;
				}
				LicenseInfoEx licInfoEx;
				licInfoEx.return_code = m_verifier->verify_limit(licInfo, er, licInfoEx.license_info);
				all_license_results.push_back(licInfoEx);
			}
		}
		result = merge_licenses(all_license_results, er, license_out);
	} else {
		result = no_license_found(er, license_out);
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

bool Licensecc::identify_pc(LCC_API_HW_IDENTIFICATION_STRATEGY pc_id_method, char* chbuffer, size_t* bufSize,
							ExecutionEnvironmentInfo* execution_environment_info) noexcept {
	bool result = false;
	if (*bufSize >= LCC_API_PC_IDENTIFIER_SIZE && chbuffer != nullptr) {
		try {
			const string pc_id = license::hw_identifier::HwIdentifierFacade::generate_user_pc_signature(pc_id_method);
			mstrlcpy(chbuffer, pc_id.c_str(), LCC_API_PC_IDENTIFIER_SIZE);
			result = true;
		} catch (const std::exception& ex) {
			LOG_ERROR("Error calculating hw_identifier: %s", ex.what());
		}
	} else {
		*bufSize = LCC_API_PC_IDENTIFIER_SIZE;
	}
	static const license::os::ExecutionEnvironment exec_env;
	if (execution_environment_info != nullptr) {
		execution_environment_info->cloud_provider = exec_env.cloud_provider();
		execution_environment_info->virtualization = exec_env.virtualization();
		execution_environment_info->virtualization_detail = exec_env.virtualization_detail();
	}
	return result;
}

static LCC_EVENT_TYPE merge_licenses(const std::vector<LicenseInfoEx>& licenses, EventRegistry& er,
									 LicenseInfo* license_out) noexcept {
	if (licenses.empty()) {
		return no_license_found(er, license_out);
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