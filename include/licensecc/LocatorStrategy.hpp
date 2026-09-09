#ifndef RETRIEVER_H_
#define RETRIEVER_H_

#include <memory>
#include <cstddef>
#include <string>
#include <vector>

#include <licensecc/datatypes.h>

namespace license {
namespace locate {
/**
 * This class provides a common interface to find and retrieve
 * licenses from different sources and positions.
 *
 * Usage:
 * <ol>
 * <li> call license_locations to get a list of available locations (the returned format is defined by the class, it's
 * usually the file name)</li> <li> iterate over the returned vector and call retrieve_license_content to get the
 * content of the license</li>
 * </ol>
 *
 * Strategies do not register events into the EventRegistry: they return a status
 * code and fill the out parameter with the locations found. Registering the
 * outcome into the EventRegistry is the responsibility of the FoundLicenseCursor.
 */
class LocatorStrategy {
protected:
	const std::string m_strategy_name;
	inline explicit LocatorStrategy(const std::string& strategyName) : m_strategy_name(strategyName) {}

public:
	const virtual std::string get_strategy_name() const { return m_strategy_name; }
	/**
	 * Try to find licenses
	 * @param license_location_out
	 * Output parameter filled with a list of identifiers for call retrieve_license_content.
	 * @return
	 * LCC_EVENT_TYPE status. LICENSE_FOUND (or LICENSE_OK) when at least one location is found,
	 * otherwise the specific failure code.
	 */
	const virtual LCC_EVENT_TYPE license_locations(std::vector<std::string>& license_location_out) = 0;

	/**
	 * Default implementation is to retrieve the license from file.
	 * Subclasses may override it.
	 * @param licenseLocationId
	 * 	String that identifies the license. It is usually the file name
	 * 	but can be whatever is understood by the class
	 * @param license_content_out
	 * 	Filled with the license data (INI format) on success.
	 * @return
	 * 	LICENSE_FOUND when the license content was successfully retrieved,
	 * 	LICENSE_FILE_NOT_FOUND otherwise.
	 */
	const virtual LCC_EVENT_TYPE retrieve_license_content(const std::string& licenseLocationId,
														  std::string& license_content_out) const;

	virtual std::unique_ptr<LocatorStrategy> clone() const = 0;
	inline virtual ~LocatorStrategy() {}
};

}  // namespace locate
}  // namespace license
#endif
