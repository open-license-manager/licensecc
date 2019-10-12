/*
 * EnvironmentVarData.cpp
 *
 *  Created on: Oct 12, 2019
 *     Author: Gabriele Contini
 */

#include "EnvironmentVarData.hpp"
namespace license {
namespace locate {

using namespace std;

EnvironmentVarData::EnvironmentVarData() :
		LocatorStrategy("EnvironmentVarData") {
}

EnvironmentVarData::~EnvironmentVarData() {
}

const vector<string> EnvironmentVarData::licenseLocations(
		EventRegistry &eventRegistry) const {
	vector<string> diskFiles;

	return diskFiles;
}

const std::string EnvironmentVarData::retrieveLicense(const std::string &licenseLocation) const{
	return "";
}

}
}
