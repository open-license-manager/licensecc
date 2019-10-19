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

#include "api/license++.h"

#include "LicenseReader.hpp"

using namespace std;
void print_error(char out_buffer[256], LicenseInfo* licenseInfo) {

}

void identify_pc(IDENTIFICATION_STRATEGY pc_id_method, char chbuffer[PC_IDENTIFIER_SIZE + 1]) {

}

static void mergeLicenses(vector<license::FullLicenseInfo> licenses,
		LicenseInfo* license) {
	if (license != nullptr) {
		time_t curLicense_exp = 0;
		for (auto it = licenses.begin(); it != licenses.end(); it++) {
			//choose the license that expires later...
			if (!it->has_expiry) {
				it->toLicenseInfo(license);
				break;
			} else if (curLicense_exp < it->expires_on()) {
				curLicense_exp = it->expires_on();
				it->toLicenseInfo(license);
			}
		}
	}
}

EVENT_TYPE acquire_license(const char * product,
		const LicenseLocation* licenseLocation, LicenseInfo* licenseInfoOut) {
	license::LicenseReader lr = license::LicenseReader(licenseLocation);
	vector<license::FullLicenseInfo> licenses;
	license::EventRegistry er = lr.readLicenses(string(product), licenses);
	EVENT_TYPE result;
	if (licenses.size() > 0) {
		vector<license::FullLicenseInfo> licenses_with_errors;
		vector<license::FullLicenseInfo> licenses_ok;
		for (auto it = licenses.begin(); it != licenses.end(); it++) {
			bool valid = it->validate(0,er);
			if (valid) {
				licenses_ok.push_back(*it);
			} else {
				licenses_with_errors.push_back(*it);
			}
		}
		if (licenses_ok.size() > 0) {
			er.turnErrorsIntoWarnings();
			result = LICENSE_OK;
			mergeLicenses(licenses_ok, licenseInfoOut);
		} else {
			er.turnWarningsIntoErrors();
			result = er.getLastFailure()->event_type;
			mergeLicenses(licenses_with_errors, licenseInfoOut);
		}

	} else {
		er.turnWarningsIntoErrors();
		result = er.getLastFailure()->event_type;
	}
#ifdef _DEBUG
	cout << er <<endl;
#endif
	if (licenseInfoOut != nullptr) {
		er.exportLastEvents(licenseInfoOut->status, AUDIT_EVENT_NUM);
	}
	return result;
}

EVENT_TYPE confirm_license(char * product,
		LicenseLocation licenseLocation) {
	return LICENSE_OK;
}

EVENT_TYPE release_license(char * product,
		LicenseLocation licenseLocation) {
	return LICENSE_OK;
}
