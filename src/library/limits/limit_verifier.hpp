/*
 * limit_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_

#include <functional>

#include <licensecc/datatypes.h>

#include "../base/base.h"
#include "../LicenseParser.hpp"

namespace license {

struct LicenseInfoEx {
	LicenseInfo license_info;
	FUNCTION_RETURN return_code;
};

/**
 * Function type of a single license limit verifier.
 *
 * A verifier is stateless with respect to the license being verified: each call
 * receives the license and the output structure. It returns the audit event
 * describing its outcome (events greater or equal to `LICENSE_OK` are OK, lower
 * values are errors). Each verifier catches its own exceptions, hence the
 * functions are noexcept.
 */
typedef std::function<LCC_EVENT_TYPE(const FullLicenseInfo& licInfo, LicenseInfo& out)> LimitVerifierFn;

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_ */