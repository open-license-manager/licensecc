/*
 * EnvironmentVarLocation.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_
#define SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_

#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class EnvironmentVarLocation: public LocatorStrategy {

public:
	EnvironmentVarLocation();
	virtual const std::vector<std::string> licenseLocations(EventRegistry& eventRegistry) const;
	virtual ~EnvironmentVarLocation();
};

}
}

#endif /* SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_ */
