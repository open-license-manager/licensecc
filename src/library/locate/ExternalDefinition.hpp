/*
 * ExplicitDefinition.hpp
 *
 *  Created on: Oct 12, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_
#define SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_

#include <memory>
#include <licensecc/LocatorStrategy.hpp>

namespace license {
namespace locate {

class ExternalDefinition : public LocatorStrategy {
private:
	const LicenseLocation* m_location;

public:
	explicit ExternalDefinition(const LicenseLocation* location);
	const virtual LCC_EVENT_TYPE license_locations(std::vector<std::string>& license_location_out);
	const virtual std::string retrieve_license_content(const std::string& licenseLocation) const;
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~ExternalDefinition();
};

} /* namespace locate */
}  // namespace license

#endif /* SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_ */
