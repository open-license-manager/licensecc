/*
 * LicenseReader.h
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifndef LICENSEREADER_H_
#define LICENSEREADER_H_
#include <string>
#include <ctime>

#define SI_SUPPORT_IOSTREAMS
#include "api/datatypes.h"
#include "base/EventRegistry.h"
#include "os/os.h"
#include "ini/SimpleIni.h"

namespace license {

class FullLicenseInfo {
public:
	std::string source;
	std::string product;
	std::string license_signature;
	int license_version;
	std::string from_date;
	std::string to_date;
	bool has_expiry;
	unsigned int from_sw_version;
	unsigned int to_sw_version;
	bool has_versions;
	std::string client_signature;
	bool has_client_sig;
	std::string extra_data;

	static const char* UNUSED_TIME;
	static const unsigned int UNUSED_SOFTWARE_VERSION = 0;

	FullLicenseInfo(const std::string& source, const std::string& product,
			const std::string& license_signature, int licenseVersion,
			std::string from_date = UNUSED_TIME,
			std::string to_date = UNUSED_TIME, //
			const std::string& client_signature = "", //
			unsigned int from_sw_version = UNUSED_SOFTWARE_VERSION,
			unsigned int to_sw_version = UNUSED_SOFTWARE_VERSION,
			const std::string& extra_data = "");
	std::string printForSign() const;
	void printAsIni(std::ostream & a_ostream) const;
	void toLicenseInfo(LicenseInfo* license) const;
	bool validate(int sw_version, EventRegistry& eventRegistryOut);
	time_t expires_on() const;
	time_t valid_from() const;
};

/**
 * This class it is responsible to read the licenses from the disk
 * (in future from network) examining all the possible LicenseLocation
 * positions.
 *
 * Each section of the ini file represents a product.
 * <pre>
 * [product]
 *  sw_version_from = (optional int)
 *  sw_version_to = (optional int)
 *  from_date = YYYY-MM-DD (optional)
 *  to_date  = YYYY-MM-DD (optional)
 *  client_signature = XXXXXXXX (optional std::string 16)
 *  license_signature = XXXXXXXXXX (mandatory, 1024)
 *  application_data = xxxxxxxxx (optional std::string 16)
 *	license_version = 100 (mandatory int)
 *  </pre>
 */
class LicenseReader {
private:
	const LicenseLocation* licenseLocation;
public:
	LicenseReader(const LicenseLocation* licenseLocation);
	EventRegistry readLicenses(const std::string &product,
			std::vector<FullLicenseInfo>& licenseInfoOut);
	virtual ~LicenseReader();
};
}
#endif /* LICENSEREADER_H_ */
