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
	const LicenseLocation* m_location;
	bool licenseDataIsBase64 = false;
public:
	ExternalDefinition(const LicenseLocation* location);
	virtual const std::vector<std::string> license_locations(EventRegistry& eventRegistry);
	virtual const std::string retrieve_license_content(const std::string &licenseLocation) const;
	virtual ~ExternalDefinition();
};

} /* namespace locate */
} /* namespace license */

#endif /* SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_ */
