/*
 * license_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LICENSE_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_LICENSE_VERIFIER_HPP_

#include <vector>

#include <licensecc/datatypes.h>
#include <licensecc/datatypes_cpp.hpp>
#include <licensecc/EventRegistry.h>

#include "../base/base.h"

namespace license {

/**
 * Runs every limit verifier, aggregates the results and registers the outcome
 * events into the shared event registry. `LICENSE_OK` is registered only when
 * every limit verifier answered with an OK event.
 */
class LicenseVerifier {
private:
	std::vector<LimitVerifierFn> m_verifiers;

public:
	/**
	 * Builds the default verifier: date + pc signature + license signature.
	 */
	LicenseVerifier();

	/**
	 * Builds a verifier from an explicit list of limit verifiers.
	 */
	explicit LicenseVerifier(const std::vector<LimitVerifierFn>& verifiers);

	FUNCTION_RETURN verify_limit(const FullLicenseInfo& licInfo, EventRegistry& event_registry, LicenseInfo& out);
	~LicenseVerifier() {}
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LICENSE_VERIFIER_HPP_ */