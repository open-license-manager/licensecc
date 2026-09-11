/*
 * ApplicationFolder.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_
#define SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_

#include <string>
#include <memory>

#include <licensecc/LocatorStrategy.hpp>

namespace license {
namespace locate {

class ApplicationFolder : public LocatorStrategy {
public:
	ApplicationFolder();
	const virtual LCC_EVENT_TYPE license_locations(std::vector<std::string>& license_location_out);
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~ApplicationFolder();
};

}  // namespace locate
} /* namespace license */

#endif /* SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_ */
