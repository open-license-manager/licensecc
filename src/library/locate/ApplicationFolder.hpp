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
	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry);
	virtual std::unique_ptr<LocatorStrategy> clone() const override;
	virtual ~ApplicationFolder();
};

}  // namespace locate
} /* namespace license */

#endif /* SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_ */
