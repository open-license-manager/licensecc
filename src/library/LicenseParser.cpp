/*
 * LicenseParser.cpp
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifdef _WIN32
#pragma warning(disable : 4786)
#else
#include <unistd.h>
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

#include <licensecc/licensecc.h>

#define SI_SUPPORT_IOSTREAMS
#include "base/base.h"
#include "LicenseParser.hpp"
#include "ini/SimpleIni.h"
#include "base/string_utils.h"
#include "base/logger.h"
#include "locate/LocatorFactory.hpp"

namespace license {
using namespace std;

LicenseParser::LicenseParser(EventRegistry& eventRegistry) : eventRegistry(eventRegistry) {}

std::vector<FullLicenseInfo> LicenseParser::parseLicense(const std::string& product,
														 const locate::RawLicenseData& rawLicense) const {
	std::vector<FullLicenseInfo> licenseInfoOut;
	const std::string& license = rawLicense.data;
	const std::string& locationId = rawLicense.location_id;
	const std::string product_up = toupper_copy(product);
	const char* productNamePtr = product_up.c_str();
	CSimpleIniA ini;
	ini.Reset();
	const SI_Error rc = ini.LoadData(license.c_str(), license.size());
	if (rc < 0) {
		eventRegistry.addEvent(FILE_FORMAT_NOT_RECOGNIZED, locationId);
		return licenseInfoOut;
	}
	const int sectionSize = ini.GetSectionSize(productNamePtr);
	if (sectionSize <= 0) {
		eventRegistry.addEvent(PRODUCT_NOT_LICENSED, locationId);
		return licenseInfoOut;
	} else {
		eventRegistry.addEvent(PRODUCT_FOUND, locationId);
	}
	/*
	 *  sw_version_from = (optional int)
	 *  sw_version_to = (optional int)
	 *  from_date = YYYY-MM-DD (optional)
	 *  to_date  = YYYY-MM-DD (optional)
	 *  client_signature = XXXX-XXXX-XXXX (optional string 16)
	 *  sig = XXXXXXXXXX (mandatory, 1024)
	 *  application_data = xxxxxxxxx (optional string 16)
	 */
	const char* license_signature = ini.GetValue(productNamePtr, LICENSE_SIGNATURE, nullptr);
	long license_version = ini.GetLongValue(productNamePtr, LICENSE_VERSION, -1);
	if (license_signature != nullptr && license_version <= 210) {
		CSimpleIniA::TNamesDepend keys;
		ini.GetAllKeys(productNamePtr, keys);
		FullLicenseInfo licInfo;
		licInfo.license_signature = license_signature;
		licInfo.source = locationId;
		licInfo.m_project = product;
		licInfo.m_magic = 0;
		for (auto& key : keys) {
			licInfo.m_limits[key.pItem] = ini.GetValue(productNamePtr, key.pItem, nullptr);
		}
		licenseInfoOut.push_back(licInfo);
	} else {
		eventRegistry.addEvent(LICENSE_MALFORMED, locationId);
	}
	return licenseInfoOut;
}

LicenseParser::~LicenseParser() {}

}  // namespace license
