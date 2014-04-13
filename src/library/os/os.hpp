/*
 * os-dependent.hpp
 *
 *  Created on: Mar 29, 2014
 *      Author: devel
 */

#ifndef OS_DEPENDENT_HPP_
#define OS_DEPENDENT_HPP_

#include <string>
#include <vector>

namespace license {
using namespace std;

enum VIRTUALIZATION {
	NONE,VMWARE
};

struct AdapterInfo {
	int id;
	string description;
	unsigned char mac_address[8];
	unsigned char ipv4_address[4];
};

struct DiskInfo {
	int id;
	string device;
	unsigned char disk_sn[8];
	string label;
};

class OsFunctions {
public:
	static vector<AdapterInfo> getAdapterInfos();
	static vector<DiskInfo> getDiskInfos();
	static string getModuleName();
	static string getUserHomePath();
	//use system dependent methods to verify signature
	static bool verifySignature(const char * stringToVerify, const char* signature);
	static VIRTUALIZATION getVirtualization();
};

}


#endif /* OS_DEPENDENT_HPP_ */
