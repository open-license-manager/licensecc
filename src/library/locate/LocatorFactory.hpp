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

public:
	// Static methods for managing extra strategies
	static void set_extra_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies);

	static FUNCTION_RETURN get_active_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategiesOut,
												 const LicenseLocation* locationHint);
	static void find_license_near_module(bool enable) { find_license_near_moduleb = enable; }
	/**
	 * Activates 2 strategies: EnvironmentVarLocation, EnvironmentVarData
	 *
	 */
	static void find_license_with_env_var(bool enable) { find_license_with_env_varb = enable; }
};

}  // namespace locate
}  // namespace license

#endif
