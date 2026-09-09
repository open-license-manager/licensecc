/*
 * EnvironmentVarLocation.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_ENVIRONMENTVARDATA_H_
#define SRC_LIBRARY_LOCATE_ENVIRONMENTVARDATA_H_

#include <memory>
#include <licensecc/LocatorStrategy.hpp>

namespace license {
namespace locate {
class EnvironmentVarData : public LocatorStrategy {
private:
	bool isBase64 = false;

public:
	EnvironmentVarData();
	const virtual LCC_EVENT_TYPE license_locations(std::vector<std::string>& license_location_out);
	const virtual LCC_EVENT_TYPE retrieve_license_content(const std::string& licenseLocation,
														  std::string& content_out) const;
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~EnvironmentVarData();
};

}  // namespace locate
}  // namespace license

#endif
