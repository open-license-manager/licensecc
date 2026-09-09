/*
 * Licensecc.hpp
 *
 *  Created on: Aug 1, 2026
 *      Author: Gabriele Contini
 */

#ifndef INCLUDE_LICENSECC_LICENSECC_HPP_
#define INCLUDE_LICENSECC_LICENSECC_HPP_

#include <memory>
#include <string>
#include <vector>

#include <licensecc/datatypes.h>
#include <licensecc/licensecc.h>
#include <licensecc/LocatorStrategy.hpp>
#include <licensecc/datatypes_cpp.hpp>

namespace license {

class LicenseVerifier;

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
	const std::vector<locate::LocatorStrategy>* m_strategies;
	std::unique_ptr<LicenseVerifier> m_verifier;

public:
	/**
	 * @brief
	 */
	Licensecc(const std::vector<locate::LocatorStrategy>* strategies_in = nullptr,
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
	 * @param identifier_out Buffer of LCC_API_PC_IDENTIFIER_SIZE characters receiving the identifier string
	 * @param execution_environment_info Optional output for execution environment info
	 * @return true if successful, false otherwise
	 */
	bool identify_pc(LCC_API_HW_IDENTIFICATION_STRATEGY hw_id_method, char identifier_out[LCC_API_PC_IDENTIFIER_SIZE],
					 ExecutionEnvironmentInfo* execution_environment_info) noexcept;
};

} /* namespace license */

#endif /* INCLUDE_LICENSECC_LICENSECC_HPP_ */