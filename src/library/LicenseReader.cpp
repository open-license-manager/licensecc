/*
 * LicenseReader.cpp
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifdef _WIN32
# pragma warning(disable: 4786)
#else
# include <unistd.h>
#endif

#include <cstring>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <math.h>

#include "pc-identifiers.h"
#include "build_properties.h"
#include "public-key.h"
#include "LicenseReader.hpp"
#include "base/StringUtils.h"
#include "base/logger.h"
#include "locate/LocatorFactory.hpp"

namespace license {

const char *FullLicenseInfo::UNUSED_TIME = "0000-00-00";

FullLicenseInfo::FullLicenseInfo(const string &source, const string &product,
		const string &license_signature, int licenseVersion, string from_date,
		string to_date, const string &client_signature,
		unsigned int from_sw_version, unsigned int to_sw_version,
		const string &extra_data) :
		source(source), product(product), //
		license_signature(license_signature), license_version(licenseVersion), //
		from_date(from_date), to_date(to_date), //
		has_expiry(to_date != UNUSED_TIME), //
		from_sw_version(from_sw_version), to_sw_version(to_sw_version), //
		has_versions(
				from_sw_version != UNUSED_SOFTWARE_VERSION
						|| to_sw_version != UNUSED_SOFTWARE_VERSION), //
		client_signature(client_signature), has_client_sig(
				client_signature.length() > 0), //
		extra_data(extra_data) {
}

EventRegistry FullLicenseInfo::validate(int sw_version) {
	EventRegistry er;
	os_initialize();
	const FUNCTION_RETURN sigVer = verifySignature(printForSign().c_str(),
			license_signature.c_str());
	const bool sigVerified = sigVer == FUNC_RET_OK;
	if (sigVerified) {
		er.addEvent(LICENSE_VERIFIED, SVRT_INFO);
	} else {
		er.addEvent(LICENSE_CORRUPTED, SVRT_ERROR);
	}
	if (has_expiry) {
		const time_t now = time(nullptr);
		if (expires_on() < now) {
			er.addEvent(PRODUCT_EXPIRED, SVRT_ERROR, "");
		}
		if (valid_from() > now) {
			er.addEvent(PRODUCT_EXPIRED, SVRT_ERROR);
		}
	}
	if (has_client_sig) {
		PcSignature str_code;
		strncpy(str_code, client_signature.c_str(), sizeof(str_code) - 1);
		const EVENT_TYPE event = validate_pc_signature(str_code);
		if (event != LICENSE_OK) {
			er.addEvent(event, SVRT_ERROR);
		}
	}
	return er;
}

void FullLicenseInfo::toLicenseInfo(LicenseInfo *license) const {
	if (license != nullptr) {
		strncpy(license->proprietary_data, extra_data.c_str(),
		PROPRIETARY_DATA_SIZE);

		license->linked_to_pc = has_client_sig;
		license->has_expiry = has_expiry;
		if (!has_expiry) {
			license->expiry_date[0] = '\0';
			license->days_left = 999999;
		} else {
			strncpy(license->expiry_date, to_date.c_str(), 11);
			const double secs = difftime(seconds_from_epoch(to_date.c_str()),
					time(nullptr));
			license->days_left = round(secs / (60 * 60 * 24));
		}
	}
}

LicenseReader::LicenseReader(const LicenseLocation &licenseLocation) :
		licenseLocation(licenseLocation) {
}

EventRegistry LicenseReader::readLicenses(const string &product,
		vector<FullLicenseInfo> &licenseInfoOut) {
	vector<string> diskFiles;
	vector<unique_ptr<locate::LocatorStrategy>> locator_strategies;
	FUNCTION_RETURN ret = locate::LocatorFactory::getActiveStrategies(
			locator_strategies, licenseLocation.licenseFileLocation);
	EventRegistry eventRegistry;
	if (ret != FUNC_RET_OK) {
		eventRegistry.addError(LICENSE_FILE_NOT_FOUND);
		return eventRegistry;
	}

	if (!eventRegistry.isGood()) {
		return eventRegistry;
	}
	bool atLeastOneFileFound = false;
	bool atLeastOneFileRecognized = false;
	bool atLeastOneProductLicensed = false;
	bool atLeastOneLicenseComplete = false;
	for (unique_ptr<locate::LocatorStrategy>& locator : locator_strategies) {
		vector<string> licenseLocations = locator->licenseLocations(
				eventRegistry);
		if (licenseLocations.size() == 0) {
			continue;
		}
		atLeastOneFileFound = true;
		CSimpleIniA ini;
		for (auto it = licenseLocations.begin(); it != licenseLocations.end();
				it++) {
			ini.Reset();
			string license = locator->retrieveLicense((*it).c_str());
			const SI_Error rc = ini.LoadData(license.c_str(), license.size());
			if (rc < 0) {
				eventRegistry.addEvent(FILE_FORMAT_NOT_RECOGNIZED, SVRT_WARN,
						*it);
				continue;
			} else {
				atLeastOneFileRecognized = true;
			}
			const char *productNamePtr = product.c_str();
			const int sectionSize = ini.GetSectionSize(productNamePtr);
			if (sectionSize <= 0) {
				eventRegistry.addEvent(PRODUCT_NOT_LICENSED, SVRT_WARN, *it);
				continue;
			} else {
				atLeastOneProductLicensed = true;
			}
			/*
			 *  sw_version_from = (optional int)
			 *  sw_version_to = (optional int)
			 *  from_date = YYYY-MM-DD (optional)
			 *  to_date  = YYYY-MM-DD (optional)
			 *  client_signature = XXXX-XXXX-XXXX-XXXX (optional string 16)
			 *  license_signature = XXXXXXXXXX (mandatory, 1024)
			 *  application_data = xxxxxxxxx (optional string 16)
			 */
			const char *license_signature = ini.GetValue(productNamePtr,
					"license_signature", nullptr);
			long license_version = ini.GetLongValue(productNamePtr,
					"license_version", -1);
			if (license_signature != nullptr && license_version > 0) {
				const string from_date = trim_copy(
						ini.GetValue(productNamePtr, "from_date",
								FullLicenseInfo::UNUSED_TIME));
				const string to_date = trim_copy(
						ini.GetValue(productNamePtr, "to_date",
								FullLicenseInfo::UNUSED_TIME));
				string client_signature = trim_copy(
						ini.GetValue(productNamePtr, "client_signature", ""));
				/*client_signature.erase(
				 std::remove(client_signature.begin(), client_signature.end(), '-'),
				 client_signature.end());*/
				const int from_sw_version = ini.GetLongValue(productNamePtr,
						"from_sw_version",
						FullLicenseInfo::UNUSED_SOFTWARE_VERSION);
				const int to_sw_version = ini.GetLongValue(productNamePtr,
						"to_sw_version",
						FullLicenseInfo::UNUSED_SOFTWARE_VERSION);
				string extra_data = trim_copy(
						ini.GetValue(productNamePtr, "extra_data", ""));
				FullLicenseInfo licInfo(*it, product, license_signature,
						(int) license_version, from_date, to_date,
						client_signature, from_sw_version, to_sw_version,
						extra_data);
				licenseInfoOut.push_back(licInfo);
				atLeastOneLicenseComplete = true;
			} else {
				eventRegistry.addEvent(LICENSE_MALFORMED, SVRT_WARN, *it);
			}
		}
	}
	if (!atLeastOneFileFound) {
		eventRegistry.turnEventIntoError(ENVIRONMENT_VARIABLE_NOT_DEFINED);
		eventRegistry.turnEventIntoError(LICENSE_FILE_NOT_FOUND);
	} else if (!atLeastOneFileRecognized) {
		eventRegistry.turnEventIntoError(FILE_FORMAT_NOT_RECOGNIZED);
	} else if (!atLeastOneProductLicensed) {
		eventRegistry.turnEventIntoError(PRODUCT_NOT_LICENSED);
	} else	if (!atLeastOneLicenseComplete) {
		eventRegistry.turnEventIntoError(LICENSE_MALFORMED);
	}
	return eventRegistry;
}

LicenseReader::~LicenseReader() {
}

string FullLicenseInfo::printForSign() const {
	ostringstream oss;
	oss << toupper_copy(trim_copy(this->product));
	oss << SHARED_RANDOM
	;
	if (has_client_sig) {
		oss << trim_copy(this->client_signature);
	}
	if (has_versions) {
		oss << "|" << this->from_sw_version << "-" << this->to_sw_version;
	}
	if (has_expiry) {
		oss << "|" << this->from_date << "|" << this->to_date;
	}
	if (this->extra_data.length() > 0) {
		oss << "|" << extra_data;
	}
#ifdef _DEBUG
	cout << "[" << oss.str() << "]" << endl;
#endif
	return oss.str();

}

void FullLicenseInfo::printAsIni(ostream &a_ostream) const {
	CSimpleIniA ini;
	string result;
	const string product = toupper_copy(trim_copy(this->product));
	CSimpleIniA::StreamWriter sw(a_ostream);
	ini.SetLongValue(product.c_str(), "license_version",
	PROJECT_INT_VERSION);
	ini.SetValue(product.c_str(), "license_signature",
			this->license_signature.c_str());
	if (has_client_sig) {
		ini.SetValue(product.c_str(), "client_signature",
				this->client_signature.c_str());
	}
	if (has_versions) {
		ini.SetLongValue(product.c_str(), "from_sw_version", from_sw_version);
		ini.SetLongValue(product.c_str(), "to_sw_version", to_sw_version);
	}

	if (this->from_date != UNUSED_TIME) {
		ini.SetValue(product.c_str(), "from_date", from_date.c_str());
	}
	if (this->to_date != UNUSED_TIME) {
		ini.SetValue(product.c_str(), "to_date", to_date.c_str());
	}
	if (this->extra_data.length() > 0) {
		ini.SetValue(product.c_str(), "extra_data", this->extra_data.c_str());
	}
	ini.Save(sw);
}

time_t FullLicenseInfo::expires_on() const {
	return seconds_from_epoch(this->to_date.c_str());
}

time_t FullLicenseInfo::valid_from() const {
	return seconds_from_epoch(this->from_date.c_str());
}

}

