/*
 * EnvironmentVarLocation.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_
#define SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_

#include <memory>
#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class EnvironmentVarLocation : public LocatorStrategy {
public:
	EnvironmentVarLocation();
	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry);
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~EnvironmentVarLocation();
};

}  // namespace locate
}  // namespace license

#endif /* SRC_LIBRARY_LOCATE_ENVIRONMENTVARLOCATION_H_ */
