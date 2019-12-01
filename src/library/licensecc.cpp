//============================================================================
// Name        : license-manager-cpp.cpp
// Author      :
// Version     :
// Copyright   : BSD
//============================================================================

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>

#include "limits/license_verifier.hpp"
#include "LicenseReader.hpp"

using namespace std;
void print_error(char out_buffer[256], LicenseInfo* licenseInfo) {}

void identify_pc(IDENTIFICATION_STRATEGY pc_id_method, char chbuffer[PC_IDENTIFIER_SIZE + 1]) {}

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

EVENT_TYPE acquire_license(const CallerInformations* callerInformation, const LicenseLocation* licenseLocation,
						   LicenseInfo* license_out) {
	const license::LicenseReader lr = license::LicenseReader(licenseLocation);
	vector<license::FullLicenseInfo> licenses;
	string project;
	if (callerInformation != nullptr && strlen(callerInformation->project_name) > 0) {
		project = string(callerInformation->project_name);
	} else {
		project = string(LCC_PROJECT_NAME);
	}
	license::EventRegistry er = lr.readLicenses(string(project), licenses);
	EVENT_TYPE result;
	if (licenses.size() > 0) {
		vector<LicenseInfo> licenses_with_errors;
		vector<LicenseInfo> licenses_ok;
		license::LicenseVerifier verifier(er);
		for (auto it = licenses.begin(); it != licenses.end(); it++) {
			FUNCTION_RETURN signatureValid = verifier.verify_signature(*it);
			if (signatureValid == FUNC_RET_OK) {
				if (verifier.verify_limits(*it) == FUNC_RET_OK) {
					licenses_ok.push_back(verifier.toLicenseInfo(*it));
				} else {
					licenses_with_errors.push_back(verifier.toLicenseInfo(*it));
				}
			} else {
				licenses_with_errors.push_back(verifier.toLicenseInfo(*it));
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
		er.exportLastEvents(license_out->status, AUDIT_EVENT_NUM);
	}
	return result;
}

EVENT_TYPE confirm_license(char* product, LicenseLocation licenseLocation) { return LICENSE_OK; }

EVENT_TYPE release_license(char* product, LicenseLocation licenseLocation) { return LICENSE_OK; }
