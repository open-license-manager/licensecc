/*
 * LicenseReader.h
 *
 *  Created on: Mar 30, 2014
 *      
 */

#ifndef LICENSEREADER_H_
#define LICENSEREADER_H_

#include "api/datatypes.h"
#include "base/EventRegistry.h"
#include "os/os.h"
#define SI_SUPPORT_IOSTREAMS
#include "ini/SimpleIni.h"
#include <string>
#include <ctime>
namespace license {

using namespace std;

class FullLicenseInfo {
public:
	string source;
	string product;
	string license_signature;
	int license_version;
	string from_date;
	string to_date;
	bool has_expiry;
	unsigned int from_sw_version;
	unsigned int to_sw_version;
	bool has_versions;
	string client_signature;
	bool has_client_sig;
	string extra_data;

	static const char* UNUSED_TIME;
	static const unsigned int UNUSED_SOFTWARE_VERSION = 0;

	FullLicenseInfo(const string& source, const string& product,
			const string& license_signature, int licenseVersion,
			string from_date = UNUSED_TIME,
			string to_date = UNUSED_TIME, //
			const string& client_signature = "", //
			unsigned int from_sw_version = UNUSED_SOFTWARE_VERSION,
			unsigned int to_sw_version = UNUSED_SOFTWARE_VERSION,
			const string& extra_data = "");
	string printForSign() const;
	void printAsIni(ostream & a_ostream) const;
	void toLicenseInfo(LicenseInfo* license) const;
	EventRegistry validate(int sw_version);
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
 *  client_signature = XXXXXXXX (optional string 16)
 *  license_signature = XXXXXXXXXX (mandatory, 1024)
 *  application_data = xxxxxxxxx (optional string 16)
 *	license_version = 100 (mandatory int)
 *  </pre>
 */
class LicenseReader {
private:
	const LicenseLocation licenseLocation;
	EventRegistry getLicenseDiskFiles(vector<string>& diskFiles);
	vector<string> filterExistingFiles(vector<string> licensePositions);
	vector<string> splitLicensePositions(string licensePositions);
	bool findLicenseWithExplicitLocation(vector<string>& diskFiles,
			EventRegistry& eventRegistry);
	bool findFileWithEnvironmentVariable(vector<string>& diskFiles,
			EventRegistry& eventRegistry);

public:
	LicenseReader(const LicenseLocation& licenseLocation);
	EventRegistry readLicenses(const string &product,
			vector<FullLicenseInfo>& licenseInfoOut);
	virtual ~LicenseReader();
};
}
#endif /* LICENSEREADER_H_ */
