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

#include <public_key.h>
#include <licensecc_properties.h>

#include "pc-identifiers.h"
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

bool FullLicenseInfo::validate(int sw_version,
		EventRegistry &eventRegistryOut) {
	os_initialize();
	const FUNCTION_RETURN sigVer = verifySignature(printForSign().c_str(),
			license_signature.c_str());
	bool is_valid = (sigVer == FUNC_RET_OK);
	if (is_valid) {
		eventRegistryOut.addEvent(SIGNATURE_VERIFIED, source);
	} else {
		eventRegistryOut.addEvent(LICENSE_CORRUPTED, source);
	}
	if (has_expiry) {
		cout<<source<<endl;
		const time_t now = time(nullptr);
		if (expires_on() < now) {
/*
			eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),
					string("Expired on: " + this->to_date).c_str());*/
			eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),nullptr);
			is_valid = false;
		}
		if (valid_from() > now) {
			/*eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),
					string("Valid from " + this->from_date).c_str());*/
			eventRegistryOut.addEvent(PRODUCT_EXPIRED, source.c_str(),nullptr);
			is_valid = false;
		}
	}
	if (has_client_sig) {
		PcSignature str_code;
		strncpy(str_code, client_signature.c_str(), sizeof(str_code) - 1);
		const EVENT_TYPE event = validate_pc_signature(str_code);
		eventRegistryOut.addEvent(event, source);
		is_valid = is_valid && (event == LICENSE_OK);
	}
	return is_valid;
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

LicenseReader::LicenseReader(const LicenseLocation* licenseLocation) :
		licenseLocation(licenseLocation) {
}

EventRegistry LicenseReader::readLicenses(const string &product,
		vector<FullLicenseInfo> &licenseInfoOut) {
	vector<string> diskFiles;
	vector<unique_ptr<locate::LocatorStrategy>> locator_strategies;
	FUNCTION_RETURN ret = locate::LocatorFactory::get_active_strategies(
			locator_strategies, licenseLocation);
	EventRegistry eventRegistry;
	if (ret != FUNC_RET_OK) {
		eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND);
		eventRegistry.turnWarningsIntoErrors();
		return eventRegistry;
	}

	bool atLeastOneLicenseComplete = false;
	for (unique_ptr<locate::LocatorStrategy> &locator : locator_strategies) {
		vector<string> licenseLocations = locator->license_locations(
				eventRegistry);
		if (licenseLocations.size() == 0) {
			continue;
		}
		CSimpleIniA ini;
		for (auto it = licenseLocations.begin(); it != licenseLocations.end();
				it++) {
			ini.Reset();
			string license = locator->retrieve_license_content((*it).c_str());
			const SI_Error rc = ini.LoadData(license.c_str(), license.size());
			if (rc < 0) {
				eventRegistry.addEvent(FILE_FORMAT_NOT_RECOGNIZED, 	*it);
				continue;
			}
			const char *productNamePtr = product.c_str();
			const int sectionSize = ini.GetSectionSize(productNamePtr);
			if (sectionSize <= 0) {
				eventRegistry.addEvent(PRODUCT_NOT_LICENSED, *it);
				continue;
			} else {
				eventRegistry.addEvent(PRODUCT_FOUND, 	*it);
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
				eventRegistry.addEvent(LICENSE_MALFORMED, *it);
			}
		}
	}
	if (!atLeastOneLicenseComplete) {
		eventRegistry.turnWarningsIntoErrors();
	}
	return eventRegistry;
}

LicenseReader::~LicenseReader() {
}

string FullLicenseInfo::printForSign() const {
	ostringstream oss;
	oss << toupper_copy(trim_copy(this->product));
	//oss << SHARED_RANDOM;
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

