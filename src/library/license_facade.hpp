/*
 * license_facade.hpp
 *
 *  Created on: Aug 1, 2026
 *      Author: Your Name
 */

#ifndef INCLUDE_LICENSECC_LICENSE_FACADE_HPP_
#define INCLUDE_LICENSECC_LICENSE_FACADE_HPP_

#include <string>
#include <vector>
#include <memory>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>

namespace license {

/**
 * @brief Facade class to unify all license library access points
 *
 * This class provides a unified interface for license acquisition and
 * hardware identification while maintaining backward compatibility
 * with the existing C API.
 */
class LicenseFacade {
public:
	/**
	 * @brief Constructor
	 */
	LicenseFacade();

	/**
	 * @brief Destructor
	 */
	virtual ~LicenseFacade();

	/**
	 * @brief Acquire a license for the specified product
	 *
	 * This method coordinates the license acquisition process by:
	 * 1. Reading licenses from various locations
	 * 2. Verifying signatures and limits
	 * 3. Selecting the best valid license
	 *
	 * @param callerInformation Optional caller information
	 * @param licenseLocation Optional license location hint
	 * @param license_out Optional output for license information
	 * @return LCC_EVENT_TYPE indicating success or failure
	 */
	LCC_EVENT_TYPE acquire_license(const CallerInformations* callerInformation, const LicenseLocation* licenseLocation,
								   LicenseInfo* license_out);

	/**
	 * @brief Calculate the hardware identifier for the current PC
	 *
	 * This method generates a unique hardware identifier based on the
	 * specified identification strategy.
	 *
	 * @param hw_id_method Strategy to use for identification
	 * @param identifier_out Buffer to receive the identifier string
	 * @param buf_size Size of the output buffer (input/output parameter)
	 * @param execution_environment_info Optional output for execution environment info
	 * @return true if successful, false otherwise
	 */
	bool identify_pc(LCC_API_HW_IDENTIFICATION_STRATEGY hw_id_method, char* identifier_out, size_t* buf_size,
					 ExecutionEnvironmentInfo* execution_environment_info);

private:
	/**
	 * @brief Helper method to merge multiple licenses into one
	 *
	 * Selects the best license based on expiration date (choosing the one
	 * that expires later).
	 *
	 * @param licenses Vector of licenses to merge
	 * @param license_out Output license information
	 */
	void mergeLicenses(const std::vector<LicenseInfo>& licenses, LicenseInfo* license_out);
};

} /* namespace license */

#endif /* INCLUDE_LICENSECC_LICENSE_FACADE_HPP_ */