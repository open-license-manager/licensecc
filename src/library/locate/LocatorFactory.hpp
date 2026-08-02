#ifndef RETRIEVE_FACTORY_H_
#define RETRIEVE_FACTORY_H_

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <licensecc/datatypes.h>
#include <licensecc_properties.h>

#include "../base/base.h"
#include "LocatorStrategy.hpp"
#include "../base/EventRegistry.h"

namespace license {
namespace locate {

struct RawLicenseData {
public:
	const std::string location_id;
	const std::string data;

	RawLicenseData(const std::string& location_id, const std::string& data) : location_id(location_id), data(data) {}
};
/**
 * Responsability:
 * * find and read a license (file) into a RawLicenseData structure. The RawLicenseData contains the text representation
 * of the file content.
 * * Act as a facade to the classes in the "locate" folder
 * * provide a configuration point for locating the licenses through its static methods (they can be called by the users
 * of the library to fine tune the way the library finds the license)
 * * provide an extension point
 *
 */
class LocatorFactory {
private:
	static bool find_license_near_moduleb;
	static bool find_license_with_env_varb;
	static std::vector<std::unique_ptr<LocatorStrategy>> extra_strategies;
	static std::vector<std::unique_ptr<LocatorStrategy>> get_active_strategies2(const LicenseLocation* locationHint);

	std::vector<std::unique_ptr<LocatorStrategy>> active_strategies;
	EventRegistry& event_registry;

public:
	// Static methods for managing extra strategies
	static void set_extra_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies);

	// Constructor that takes location hint and event registry reference
	LocatorFactory(const LicenseLocation* locationHint, EventRegistry& eventRegistryRef);

	// Static methods for backward compatibility DEPRECATED
	static FUNCTION_RETURN get_active_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategiesOut,
												 const LicenseLocation* locationHint) __attribute__((deprecated));
	static void find_license_near_module(bool enable) { find_license_near_moduleb = enable; }
	static void find_license_with_env_var(bool enable) { find_license_with_env_varb = enable; }

	// RawLicenseCursor inner class to iterate over found licenses
	class RawLicenseCursor {
	private:
		const LocatorFactory& parent_factory;
		size_t current_strategy_idx;
		std::vector<std::string> current_locations;
		size_t current_location_idx;

	public:
		// Iterator support
		RawLicenseCursor(LocatorFactory& factory);

		RawLicenseCursor& operator++();	 // Prefix increment
		RawLicenseData operator*() const;  // Dereference operator
		bool operator!=(const RawLicenseCursor& other) const;  // Comparison operator
		bool operator==(const RawLicenseCursor& other) const;  // Comparison operator

		// Friend declaration for begin/end support
		friend class LocatorFactory;
	};

	// Iterator support for the container-like behavior
	RawLicenseCursor begin();
	RawLicenseCursor end();
};

}  // namespace locate
}  // namespace license

#endif
