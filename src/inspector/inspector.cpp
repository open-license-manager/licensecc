#include <iostream>
#include <map>
#include <unordered_map>
#include <licensecc/licensecc.h>
#include <fstream>
#include "../library/os/cpu_info.hpp"

using namespace std;
const map<LCC_API_IDENTIFICATION_STRATEGY, string> stringByStrategyId = {
	{STRATEGY_DEFAULT, "DEFAULT"}, {STRATEGY_ETHERNET, "MAC"},	 {STRATEGY_IP_ADDRESS, "IP"},
	{STRATEGY_DISK_NUM, "Disk1"},  {STRATEGY_DISK_LABEL, "Disk2"}, {STRATEGY_NONE, "Custom"}};

const unordered_map<LCC_EVENT_TYPE, string> stringByEventType = {
	{LICENSE_OK, "OK "},
	{LICENSE_FILE_NOT_FOUND, "license file not found "},
	{LICENSE_SERVER_NOT_FOUND, "license server can't be contacted "},
	{ENVIRONMENT_VARIABLE_NOT_DEFINED, "environment variable not defined "},
	{FILE_FORMAT_NOT_RECOGNIZED, "license file has invalid format (not .ini file) "},
	{LICENSE_MALFORMED, "some mandatory field are missing, or data can't be fully read. "},
	{PRODUCT_NOT_LICENSED, "this product was not licensed "},
	{PRODUCT_EXPIRED, "license expired "},
	{LICENSE_CORRUPTED, "license signature didn't match with current license "},
	{IDENTIFIERS_MISMATCH, "Calculated identifier and the one provided in license didn't match"}};

static LCC_EVENT_TYPE verifyLicense(const string& fname) {
	LicenseInfo licenseInfo;
	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(fname.begin(), fname.end(), licLocation.licenseData);
	LCC_EVENT_TYPE result = acquire_license(nullptr, &licLocation, &licenseInfo);
	if (result == LICENSE_OK) {
		cout << "license OK" << endl;
	} else {
		cerr << stringByEventType.find(result)->second << endl;
	}

	return result;
}

int main(int argc, char* argv[]) {
	license::os::CpuInfo cpu;
	cout << "CpuVendor      :" << cpu.vendor() << endl;
	cout << "Virtual machine:" << cpu.cpu_virtual() << endl;
	cout << "Cpu model      : 0x" << std::hex << ((long)cpu.model()) << std::dec << endl;

	char hw_identifier[LCC_API_PC_IDENTIFIER_SIZE + 1];
	size_t bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	for (const auto& x : stringByStrategyId) {
		if (identify_pc(x.first, hw_identifier, &bufSize)) {
			std::cout << x.second << ':' << hw_identifier << std::endl;

		} else {
			std::cout << x.second << ": NA" << endl;
		}
	}

	if (argc == 2) {
		const string fname(argv[1]);
		ifstream license_file(fname);
		if (license_file.good()) {
			verifyLicense(fname);
		} else {
			cerr << "license file :" << fname << " not found." << endl;
		}
	}
}
