/*
 * ExplicitDefinition.hpp
 *
 *  Created on: Oct 12, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_
#define SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_

#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class ExternalDefinition: public LocatorStrategy {
private:
	const std::string m_location;
public:
	ExternalDefinition(const char* location);
	virtual const std::vector<std::string> licenseLocations(EventRegistry& eventRegistry) const;
	virtual ~ExternalDefinition();
};

} /* namespace locate */
} /* namespace license */

#endif /* SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_ */
