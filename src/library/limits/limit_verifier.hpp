/*
 * limit_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_

#include <memory>

#include <licensecc/datatypes.h>

#include "../base/base.h"
#include "../LicenseParser.hpp"

namespace license {

struct LicenseInfoEx {
	LicenseInfo license_info;
	FUNCTION_RETURN return_code;
};

/**
 * Abstract interface for a single license limit verifier.
 *
 * Concrete verifiers are cloneable and stateless with respect to the license
 * being verified: each `verify_limit` call receives the license and the output
 * structure. They return the audit event describing their outcome (events
 * greater or equal to `LICENSE_OK` are OK, lower values are errors). Each
 * verifier catches its own exceptions, hence `verify_limit` is noexcept.
 */
class LimitVerifier {
public:
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept = 0;
	virtual std::unique_ptr<LimitVerifier> clone() const = 0;
	virtual ~LimitVerifier();
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LIMIT_VERIFIER_HPP_ */