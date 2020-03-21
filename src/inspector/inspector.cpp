#include <iostream>
#include <map>
#include <unordered_map>
#include <licensecc/licensecc.h>
#include <fstream>
#include "../library/os/cpu_info.hpp"
#include "../library/os/execution_environment.hpp"

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

const unordered_map<int, string> descByVirt = {
	{VIRTUALIZATION::NONE, "No virtualization"}, {VIRTUALIZATION::VM, "VM"}, {VIRTUALIZATION::CONTAINER, "Container"}};

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
	license::os::CpuInfo cpu;
	cout << "Cpu Vendor      :" << cpu.vendor() << endl;
	cout << "Cpu Brand       :" << cpu.brand() << endl;
	cout << "Cpu is hypervis.:" << cpu.cpu_virtual() << endl;
	cout << "Cpu model       :0x" << std::hex << ((long)cpu.model()) << std::dec << endl;
	cout << "Virt. detail cpu:" << descByVirtDetail.find(cpu.virtualization_details())->second << endl <<endl;
	ExecutionEnvironment execEnv;
	cout << "Running in cloud:" << execEnv.is_cloud() << endl;
	cout << "Docker          :" << execEnv.is_docker() << endl;
	cout << "other container :" << execEnv.is_container() << endl;
	cout << "Virtualiz. class:" << descByVirt.find(execEnv.getVirtualization())->second << endl;

	cout << "Bios vendor     :" << execEnv.bios_vendor() << endl;
	cout << "Bios description:" << execEnv.bios_description() << endl;
	cout << "System vendor   :" << execEnv.sys_vendor() << endl <<endl;

	char hw_identifier[LCC_API_PC_IDENTIFIER_SIZE + 1];
	size_t bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	for (const auto& x : stringByStrategyId) {
		if (identify_pc(static_cast<LCC_API_HW_IDENTIFICATION_STRATEGY>(x.first), hw_identifier, &bufSize)) {
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
