#ifndef RETRIEVE_FACTORY_H_
#define RETRIEVE_FACTORY_H_

#include <cstddef>
#include <string>
#include <vector>

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

	static FUNCTION_RETURN getActiveStrategies(
			std::vector<std::unique_ptr<LocatorStrategy>> &strategiesOut,
			const char *locationHint);

};

}
}

#endif
