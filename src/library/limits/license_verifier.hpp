/*
 * LicenseVerifier.hpp
 *
 *  Created on: Nov 17, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_
#include "../base/EventRegistry.h"
#include "../LicenseParser.hpp"

namespace license {

struct LicenseInfoEx {
	LicenseInfo license_info;
	FUNCTION_RETURN return_code;
};

class LicenseVerifier {
private:
	EventRegistry& m_event_registry;

	FUNCTION_RETURN verify_signature(const FullLicenseInfo& licInfo);
	FUNCTION_RETURN verify_limits(const FullLicenseInfo& licInfo);
	LicenseInfo toLicenseInfo(const FullLicenseInfo& fullLicInfo) const;

public:
	explicit LicenseVerifier(EventRegistry& er);
	LicenseInfoEx verify_license(const FullLicenseInfo& licInfo) noexcept;
	virtual ~LicenseVerifier();
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_ */
