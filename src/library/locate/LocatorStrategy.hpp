#ifndef RETRIEVER_H_
#define RETRIEVER_H_

#include <memory>
#include <cstddef>
#include <string>
#include <vector>

#include "../base/EventRegistry.h"

namespace license {
namespace locate {
/**
 * This class provides a common interface to find and retrieve
 * licenses from different sources and positions.
 *
 * Usage:
 * <ol>
 * <li> call licenseLocations to get a list of available locations (the returned format is defined by the class, it's usually the file name)</li>
 * <li> iterate over the returned vector and call retrieveLicense to get the content of the license</li>
 * </ol>
 */
class LocatorStrategy {
protected:
	const std::string m_strategy_name;
	inline LocatorStrategy(const std::string &strategyName) :
			m_strategy_name(strategyName) {
	}
public:

	virtual const std::string get_strategy_name() const {
		return m_strategy_name;
	}

	virtual const std::vector<std::string> licenseLocations(
			EventRegistry &eventRegistry) const = 0;

	/**
	 * Default implementation is to retrieve the license from file.
	 * Subclasses may override it.
	 * @param licenseLocation
	 * @return
	 */
	virtual const std::string retrieveLicense(
			const std::string &licenseLocation) const;
	inline virtual ~LocatorStrategy() {
	}
};

}
}
#endif
