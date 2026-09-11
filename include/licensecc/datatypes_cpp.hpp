/*
 * datatypes_cpp.hpp
 *
 * C++-only data types of the licensecc public C++ API.
 *
 * Kept separate from datatypes.h (pure C data types shared with the C API).
 */

#ifndef INCLUDE_LICENSECC_DATATYPES_CPP_HPP_
#define INCLUDE_LICENSECC_DATATYPES_CPP_HPP_

#include <functional>
#include <map>
#include <string>

#include <licensecc/datatypes.h>

namespace license {

/**
 * Parsed license, as handed to the license verifiers.
 *
 * Contains the raw limits read from a license file section together with the
 * license signature and the source location it was read from.
 */
struct FullLicenseInfo {
	std::string license_signature;
	std::string source;
	std::string m_project;
	unsigned int m_magic;
	std::map<std::string, std::string> m_limits;
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

#endif /* INCLUDE_LICENSECC_DATATYPES_CPP_HPP_ */