//============================================================================
// Name        : licensecc.cpp
// Author      :
// Version     :
// Copyright   : BSD
//============================================================================

#define __STDC_WANT_LIB_EXT1__ 1
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include <licensecc_properties.h>

#include "base/logger.h"
#include "pc_identifier/pc_identifier_facade.hpp"
#include "limits/license_verifier.hpp"
#include "base/StringUtils.h"
#include "LicenseReader.hpp"

using namespace std;


void print_error(char out_buffer[LCC_API_ERROR_BUFFER_SIZE], LicenseInfo* licenseInfo) {}

bool identify_pc(LCC_API_IDENTIFICATION_STRATEGY pc_id_method, char* chbuffer, size_t* bufSize) {
	bool result = false;
	if (*bufSize > LCC_API_PC_IDENTIFIER_SIZE && chbuffer != nullptr) {
		try {
			string pc_id = license::PcIdentifierFacade::generate_user_pc_signature(pc_id_method);
			strncpy(chbuffer, pc_id.c_str(), *bufSize);
			result = true;
		} catch (const std::exception& ex) {
			LOG_ERROR("Error calculating pc_identifier: %s", ex.what());
#ifdef _DEBUG
				cout
				<< "Error occurred: " << ex.what() << std::endl;
#endif
		}
	} else {
		*bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	}
	return result;
}

static void mergeLicenses(const vector<LicenseInfo>& licenses, LicenseInfo* license_out) {
	if (license_out != nullptr) {
		int days_left = INT_MIN;
		for (auto it = licenses.begin(); it != licenses.end(); it++) {
			// choose the license that expires later...
			if (!it->has_expiry) {
				*license_out = *it;
				break;
			} else if (days_left < (int)it->days_left) {
				*license_out = *it;
				days_left = it->days_left;
			}
		}
	}
}

LCC_EVENT_TYPE acquire_license(const CallerInformations* callerInformation,
									 const LicenseLocation* licenseLocation, LicenseInfo* license_out) {
	const license::LicenseReader lr = license::LicenseReader(licenseLocation);
	vector<license::FullLicenseInfo> licenses;
	string project;
	size_t str_size;
	if (callerInformation != nullptr &&
		(str_size = license::mstrnlen_s(callerInformation->project_name, sizeof callerInformation->project_name)) > 0) {
		project = string(callerInformation->project_name, str_size);
	} else {
		project = string(LCC_PROJECT_NAME);
	}
	license::EventRegistry er = lr.readLicenses(string(project), licenses);
	LCC_EVENT_TYPE result;
	if (licenses.size() > 0) {
		vector<LicenseInfo> licenses_with_errors;
		vector<LicenseInfo> licenses_ok;
		license::LicenseVerifier verifier(er);
		for (auto full_lic_info_it = licenses.begin(); full_lic_info_it != licenses.end(); full_lic_info_it++) {
			if (callerInformation != nullptr) {
				full_lic_info_it->m_magic = callerInformation->magic;
			}
			const FUNCTION_RETURN signatureValid = verifier.verify_signature(*full_lic_info_it);
			LicenseInfo licInfo = verifier.toLicenseInfo(*full_lic_info_it);
			if (signatureValid == FUNC_RET_OK) {
				if (verifier.verify_limits(*full_lic_info_it) == FUNC_RET_OK) {
					licenses_ok.push_back(licInfo);
				} else {
					licenses_with_errors.push_back(licInfo);
				}
			} else {
				licenses_with_errors.push_back(licInfo);
			}
		}
		if (licenses_ok.size() > 0) {
			er.turnErrorsIntoWarnings();
			result = LICENSE_OK;
			mergeLicenses(licenses_ok, license_out);
		} else {
			er.turnWarningsIntoErrors();
			result = er.getLastFailure()->event_type;
			mergeLicenses(licenses_with_errors, license_out);
		}
	} else {
		er.turnWarningsIntoErrors();
		result = er.getLastFailure()->event_type;
		if (license_out != nullptr) {
			license_out->proprietary_data[0] = '\0';
			license_out->linked_to_pc = false;
			license_out->days_left = 0;
		}
	}
#ifdef _DEBUG
	cout << er << endl;
#endif

	if (license_out != nullptr) {
		er.exportLastEvents(license_out->status, LCC_API_AUDIT_EVENT_NUM);
	}
	return result;
}

LCC_EVENT_TYPE confirm_license(char* product, LicenseLocation licenseLocation) { return LICENSE_OK; }

LCC_EVENT_TYPE release_license(char* product, LicenseLocation licenseLocation) { return LICENSE_OK; }

