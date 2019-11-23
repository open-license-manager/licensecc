#ifndef RETRIEVE_FACTORY_H_
#define RETRIEVE_FACTORY_H_

#include <cstddef>
#include <string>
#include <vector>
#include <licensecc/datatypes.h>

#include "../base/base.h"
#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class LocatorFactory {
private:
	inline LocatorFactory() {
	}
	inline ~LocatorFactory() {
	}
public:

	static FUNCTION_RETURN get_active_strategies(
			std::vector<std::unique_ptr<LocatorStrategy>> &strategiesOut,
			const LicenseLocation *locationHint);

};

}
}

#endif
