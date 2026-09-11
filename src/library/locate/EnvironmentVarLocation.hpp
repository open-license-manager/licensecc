/*
 * EnvironmentVarLocation.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_
#define SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_

#include <memory>
#include <licensecc/LocatorStrategy.hpp>

namespace license {
namespace locate {

class EnvironmentVarLocation : public LocatorStrategy {
public:
	EnvironmentVarLocation();
	const virtual LCC_EVENT_TYPE license_locations(std::vector<std::string>& license_location_out);
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~EnvironmentVarLocation();
};

}  // namespace locate
}  // namespace license

#endif /* SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_ */
