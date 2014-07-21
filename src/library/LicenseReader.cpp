/*
 * LicenseReader.cpp
 *
 *  Created on: Mar 30, 2014
 *      Author: devel
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
#include "pc-identifiers.h"
#include "LicenseReader.h"
#include "base/StringUtils.h"
#include "base/public-key.h"
#include <build_properties.h>

#include "os/os-cpp.h"

namespace license {

FullLicenseInfo::FullLicenseInfo(const string& source, const string& product,
		const string& license_signature, int licenseVersion, string from_date,
		string to_date, const string& client_signature,
		unsigned int from_sw_version, unsigned int to_sw_version,
		const string& extra_data) :
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
	bool sigVerified = OsFunctions::verifySignature(printForSign().c_str(),
			license_signature.c_str());
	if (sigVerified) {
		er.addEvent(LICENSE_VERIFIED, INFO);
	} else {
		er.addEvent(LICENSE_CORRUPTED, SEVERITY_ERROR);
	}
	if (has_expiry) {
		time_t now = time(NULL);
		if (expires_on() < now) {
			er.addEvent(PRODUCT_EXPIRED, SEVERITY_ERROR, "");
		}
		if (valid_from() > now) {
			er.addEvent(PRODUCT_EXPIRED, SEVERITY_ERROR);
		}
	}
	if (has_client_sig) {
		UserPcIdentifier str_code;
		strncpy(str_code, client_signature.c_str(), sizeof(str_code));
		EVENT_TYPE event = validate_user_pc_identifier(str_code);
		if (event != LICENSE_OK) {
			er.addEvent(event, SEVERITY_ERROR);
		}
	}
	return er;
}

void FullLicenseInfo::toLicenseInfo(LicenseInfo* license) const {
	if (license != NULL) {
		strncpy(license->proprietary_data, extra_data.c_str(),
		PROPRIETARY_DATA_SIZE);

		license->linked_to_pc = has_client_sig;
		license->has_expiry = has_expiry;
		if (!has_expiry) {
			license->expiry_date[0] = '\0';
			license->days_left = 999999;
		} else {
			strncpy(license->expiry_date, to_date.c_str(), 11);
			double secs = difftime(time(NULL),
					seconds_from_epoch(to_date.c_str()));
			license->days_left = (int) secs / 60 * 60 * 24;
		}
	}
}
LicenseReader::LicenseReader(const LicenseLocation& licenseLocation) :
		licenseLocation(licenseLocation) {

}

EventRegistry LicenseReader::readLicenses(const string &product,
		vector<FullLicenseInfo>& licenseInfoOut) {
	vector<string> diskFiles;
	EventRegistry result = getLicenseDiskFiles(diskFiles);
	if (!result.isGood()) {
		return result;
	}
	bool loadAtLeastOneFile = false;
	bool atLeastOneProductLicensed = false;
	bool atLeastOneLicenseComplete = false;
	CSimpleIniA ini;
	for (auto it = diskFiles.begin(); it != diskFiles.end(); it++) {
		ini.Reset();
		SI_Error rc = ini.LoadFile((*it).c_str());
		if (rc < 0) {
			result.addEvent(FILE_FORMAT_NOT_RECOGNIZED, SEVERITY_WARN, *it);
			continue;
		} else {
			loadAtLeastOneFile = true;
		}
		const char* productNamePtr = product.c_str();
		int sectionSize = ini.GetSectionSize(productNamePtr);
		if (sectionSize <= 0) {
			result.addEvent(PRODUCT_NOT_LICENSED, SEVERITY_WARN, *it);
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
		const char * license_signature = ini.GetValue(productNamePtr,
				"license_signature", NULL);
		long license_version = ini.GetLongValue(productNamePtr,
				"license_version", -1);
		if (license_signature != NULL && license_version > 0) {
			string from_date = trim_copy(
					ini.GetValue(productNamePtr, "from_date",
							FullLicenseInfo::UNUSED_TIME));
			string to_date = trim_copy(
					ini.GetValue(productNamePtr, "to_date",
							FullLicenseInfo::UNUSED_TIME));
			string client_signature = trim_copy(
					ini.GetValue(productNamePtr, "client_signature", ""));
			client_signature.erase(
					std::remove(client_signature.begin(), client_signature.end(), '-'),
					client_signature.end());
			int from_sw_version = ini.GetLongValue(productNamePtr,
					"from_sw_version", FullLicenseInfo::UNUSED_SOFTWARE_VERSION);
			int to_sw_version = ini.GetLongValue(productNamePtr,
					"to_sw_version", FullLicenseInfo::UNUSED_SOFTWARE_VERSION);
			FullLicenseInfo licInfo(*it, product, license_signature,
					(int) license_version, from_date, to_date,
					client_signature,from_sw_version,to_sw_version);
			licenseInfoOut.push_back(licInfo);
			atLeastOneLicenseComplete = true;
		} else {
			result.addEvent(LICENSE_MALFORMED, SEVERITY_WARN, *it);
		}
	}
	if (!loadAtLeastOneFile) {
		result.turnEventIntoError(FILE_FORMAT_NOT_RECOGNIZED);
	}
	if (!atLeastOneProductLicensed) {
		result.turnEventIntoError(PRODUCT_NOT_LICENSED);
	}
	if (!atLeastOneLicenseComplete) {
		result.turnEventIntoError(LICENSE_MALFORMED);
	}
	return result;
}

bool LicenseReader::findLicenseWithExplicitLocation(vector<string>& diskFiles,
		EventRegistry& eventRegistry) {
//bool hasFileLocation = false;
	bool licenseFoundWithExplicitLocation = false;
	if (licenseLocation.licenseFileLocation != NULL
			&& licenseLocation.licenseFileLocation[0] != '\0') {
		//hasFileLocation = true;
		string varName(licenseLocation.licenseFileLocation);
		vector<string> declared_positions = splitLicensePositions(varName);
		vector<string> existing_pos = filterExistingFiles(declared_positions);
		if (existing_pos.size() > 0) {
			if (existing_pos.size() > 0) {
				licenseFoundWithExplicitLocation = true;
				for (auto it = existing_pos.begin(); it != existing_pos.end();
						++it) {
					diskFiles.push_back(*it);
					eventRegistry.addEvent(LICENSE_FILE_FOUND, INFO, *it);
				}
			}
		} else {
			eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND, SEVERITY_WARN,
					varName);
		}
	}
	return licenseFoundWithExplicitLocation;
}

bool LicenseReader::findFileWithEnvironmentVariable(vector<string>& diskFiles,
		EventRegistry& eventRegistry) {
	bool licenseFileFoundWithEnvVariable = false;
	if (licenseLocation.environmentVariableName != NULL
			&& licenseLocation.environmentVariableName[0] != '\0') {
		string varName(licenseLocation.environmentVariableName);
		if (varName.length() > 0) {
			//var name is passed in by the calling application.
			char* env_var_value = getenv(varName.c_str());
			if (env_var_value != NULL && env_var_value[0] != '\0') {
				vector<string> declared_positions = splitLicensePositions(
						string(env_var_value));
				vector<string> existing_pos = filterExistingFiles(
						declared_positions);
				if (existing_pos.size() > 0) {
					licenseFileFoundWithEnvVariable = true;
					for (auto it = existing_pos.begin();
							it != existing_pos.end(); ++it) {
						diskFiles.push_back(*it);
						eventRegistry.addEvent(LICENSE_FILE_FOUND, INFO, *it);
					}
				} else {
					eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND,
							SEVERITY_WARN, env_var_value);
				}
			} else {
				eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED,
						SEVERITY_WARN);
			}
		} else {
			eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED,
					SEVERITY_WARN);
		}
	}
	return licenseFileFoundWithEnvVariable;
}

EventRegistry LicenseReader::getLicenseDiskFiles(vector<string>& diskFiles) {
	EventRegistry eventRegistry;
	bool licenseFoundWithExplicitLocation = findLicenseWithExplicitLocation(
			diskFiles, eventRegistry);
	bool foundNearModule = false;
	if (licenseLocation.openFileNearModule) {
		string temptativeLicense = OsFunctions::getModuleName() + ".lic";
		ifstream f(temptativeLicense.c_str());
		if (f.good()) {
			foundNearModule = true;
			diskFiles.push_back(temptativeLicense);
			eventRegistry.addEvent(LICENSE_FILE_FOUND, INFO, temptativeLicense);
		} else {
			eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND, SEVERITY_WARN,
					temptativeLicense);
		}
		f.close();
	}
	bool licenseFileFoundWithEnvVariable = findFileWithEnvironmentVariable(
			diskFiles, eventRegistry);

	if (!foundNearModule && !licenseFoundWithExplicitLocation
			&& !licenseFileFoundWithEnvVariable) {
		eventRegistry.turnEventIntoError(ENVIRONMENT_VARIABLE_NOT_DEFINED);
		eventRegistry.turnEventIntoError(LICENSE_FILE_NOT_FOUND);
	}
	return eventRegistry;
}

vector<string> LicenseReader::filterExistingFiles(
		vector<string> licensePositions) {
	vector<string> existingFiles;
	for (auto it = licensePositions.begin(); it != licensePositions.end();
			it++) {
		ifstream f(it->c_str());
		if (f.good()) {
			existingFiles.push_back(*it);
		}
		f.close();
	}
	return existingFiles;
}

vector<string> LicenseReader::splitLicensePositions(string licensePositions) {
	std::stringstream streamToSplit(licensePositions);
	std::string segment;
	std::vector<string> seglist;

	while (std::getline(streamToSplit, segment, ';')) {
		seglist.push_back(segment);
	}
	return seglist;
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

void FullLicenseInfo::printAsIni(ostream & a_ostream) const {
	CSimpleIniA ini;
	string result;
	string product = toupper_copy(trim_copy(this->product));
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

