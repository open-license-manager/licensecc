/*
 * Licensecc.hpp
 *
 *  Created on: Aug 1, 2026
 *      Author: Gabriele Contini
 */

#ifndef INCLUDE_LICENSECC_LICENSECC_HPP_
#define INCLUDE_LICENSECC_LICENSECC_HPP_

#include <string>
#include <vector>
#include <memory>
#include "locate/LocatorStrategy.hpp"
#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include "limits/limit_verifier.hpp"  // Include for LicenseInfoEx
#include "limits/license_verifier.hpp"

namespace license {

/**
 * @brief Facade class to unify all license library access points
 *
 * This class provides a unified interface for license acquisition and
 * hardware identification while maintaining backward compatibility
 * with the existing C API.
 *
 * The license acquisition flow (locator strategies loop, parsing,
 * verification and merge) is described in the "License acquisition flow"
 * documentation page: doc/analysis/license_acquisition_flow.rst in this
 * repository, rendered at
 * https://open-license-manager.github.io/licensecc/latest/ (Analysis section).
 */
class Licensecc {
private:
	const std::vector<std::unique_ptr<locate::LocatorStrategy>>* m_strategies;
	LicenseVerifier m_verifier;

public:
	/**
	 * @brief
	 */
	Licensecc(const std::vector<std::unique_ptr<locate::LocatorStrategy>>* strategies_in = nullptr,
			  const std::vector<LimitVerifierFn>& extra_verifiers = std::vector<LimitVerifierFn>());

	/**
	 * @brief Destructor
	 */
	virtual ~Licensecc();

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
								   LicenseInfo* license_out) noexcept;

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
					 ExecutionEnvironmentInfo* execution_environment_info) noexcept;

private:
	/**
	 * @brief Helper method to merge multiple licenses into one
	 *
	 * Selects the best license based on expiration date (choosing the one
	 * that expires later).
	 *
	 * @param licenses Vector of extended license information with return codes
	 * @param license_out Output license information
	 * @return LCC_EVENT_TYPE indicating success or failure
	 */
	LCC_EVENT_TYPE mergeLicenses(const std::vector<LicenseInfoEx>& licenses, EventRegistry& er,
								 LicenseInfo* license_out) noexcept;
};

} /* namespace license */

#endif /* INCLUDE_LICENSECC_LICENSECC_HPP_ */