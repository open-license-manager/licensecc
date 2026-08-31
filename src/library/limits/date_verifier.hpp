/*
 * date_verifier.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_DATE_VERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_DATE_VERIFIER_HPP_

#include "limit_verifier.hpp"

namespace license {

/**
 * Verifies the license begin/expiry dates and populates the date fields of
 * `LicenseInfo`.
 */
class DateVerifier : public LimitVerifier {
public:
	DateVerifier() {}
	virtual LCC_EVENT_TYPE verify_limit(const FullLicenseInfo& licInfo, LicenseInfo& out) noexcept override;
	virtual std::unique_ptr<LimitVerifier> clone() const override;
	virtual ~DateVerifier() {}
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_DATE_VERIFIER_HPP_ */