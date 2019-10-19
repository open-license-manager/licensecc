/*
 * ApplicationFolder.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_
#define SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_

#include <string>

#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class ApplicationFolder: public LocatorStrategy {
public:
	ApplicationFolder();
	virtual const std::vector<std::string> license_locations(EventRegistry& eventRegistry);
	virtual ~ApplicationFolder();
};

}
} /* namespace license */

#endif /* SRC_LIBRARY_RETRIEVERS_APPLICATIONFOLDER_H_ */
