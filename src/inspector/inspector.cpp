#include <iostream>
#include <map>
#include <unordered_map>
#include <licensecc/licensecc.h>
#include <fstream>
#include "../library/os/cpu_info.hpp"
#include "../library/os/dmi_info.hpp"

using namespace std;
using namespace license::os;

const map<int, string> stringByStrategyId = {{STRATEGY_DEFAULT, "DEFAULT"},
											 {STRATEGY_ETHERNET, "MAC"},
											 {STRATEGY_IP_ADDRESS, "IP"},
											 {STRATEGY_DISK_NUM, "Disk1"},
											 {STRATEGY_DISK_LABEL, "Disk2"}};

const unordered_map<int, string> descByVirtDetail = {{BARE_TO_METAL, "No virtualization"},
													 {VMWARE, "Vmware"},
													 {VIRTUALBOX, "Virtualbox"},
													 {V_XEN, "XEN"},
													 {KVM, "KVM"},
													 {HV, "Microsoft Hypervisor"},
													 {V_OTHER, "Other type of vm"}};

const unordered_map<LCC_API_VIRTUALIZATION_SUMMARY, string> descByVirt = {
	{LCC_API_VIRTUALIZATION_SUMMARY::NONE, "No virtualization"},
	{LCC_API_VIRTUALIZATION_SUMMARY::VM, "VM"},
	{LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER, "Container"}};

const unordered_map<int, string> stringByEventType = {
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
	char hw_identifier[LCC_API_PC_IDENTIFIER_SIZE + 1];
	size_t bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	ExecutionEnvironmentInfo exec_env_info;
	for (const auto& x : stringByStrategyId) {
		if (identify_pc(static_cast<LCC_API_HW_IDENTIFICATION_STRATEGY>(x.first), hw_identifier, &bufSize,
						&exec_env_info)) {
			std::cout << x.second << ':' << hw_identifier << std::endl;
		} else {
			std::cout << x.second << ": NA" << endl;
		}
	}
	cout << "Virtualiz. class :" << descByVirt.find(exec_env_info.virtualization)->second << endl;
	cout << "Virtualiz. detail:" << descByVirtDetail.find(exec_env_info.virtualization_detail)->second << endl;
	cout << "Cloud provider   :" << exec_env_info.cloud_provider << endl << "=============" << endl;
	;
	license::os::CpuInfo cpu;
	cout << "Cpu Vendor       :" << cpu.vendor() << endl;
	cout << "Cpu Brand        :" << cpu.brand() << endl;
	cout << "Cpu hypervisor   :" << cpu.is_hypervisor_set() << endl;
	cout << "Cpu model        :0x" << std::hex << ((long)cpu.model()) << std::dec << endl;
	license::os::DmiInfo dmi_info;

	cout << "Bios vendor     :" << dmi_info.bios_vendor() << endl;
	cout << "Bios description:" << dmi_info.bios_description() << endl;
	cout << "System vendor   :" << dmi_info.sys_vendor() << endl << endl;


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
