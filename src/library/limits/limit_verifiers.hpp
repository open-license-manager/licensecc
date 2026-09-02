/*
 * limit_verifiers.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LIMIT_VERIFIERS_HPP_
#define SRC_LIBRARY_LIMITS_LIMIT_VERIFIERS_HPP_

#include "limit_verifier.hpp"

namespace license {

/**
 * Verifies the license begin/expiry dates and populates the date fields of
 * `LicenseInfo`.
 */
LCC_EVENT_TYPE verify_date(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept;

/**
 * Verifies the hardware (client) signature of the license against the
 * signature generated from the current machine.
 */
LCC_EVENT_TYPE verify_pc_signature(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept;

/**
 * Verifies the cryptographic signature of the license file.
 */
LCC_EVENT_TYPE verify_signature(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept;

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LIMIT_VERIFIERS_HPP_ */