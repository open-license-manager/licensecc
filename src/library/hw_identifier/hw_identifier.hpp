/*
 * hw_identifier.h
 *
 *  Created on: Dec 22, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_

#include <array>
#include <iostream>
#include <string>

#include <licensecc_properties.h>
#include "../../../include/licensecc/datatypes.h"
#include "../os/execution_environment.hpp"
#include "../os/cpu_info.hpp"

namespace license {
namespace hw_identifier {

/**
 * @brief Stores a hardware identifier composed of a 1 bit header and an 8-byte payload.
 *
 * The 9 bytes of #m_data encode the identification strategy, environment
 * flags, and the strategy-specific proprietary data.  The layout is:
 *
 * - **m_data[0] bit 7** — License source flag (0 = being generated,
 *   1 = from an issued license).  Currently unused.
 * - **m_data[0] bit 6** — Set when the identifier was produced using an
 *   environment variable override.
 * - **m_data[0] bits 5-3** — Identification strategy
 *   (#LCC_API_HW_IDENTIFICATION_STRATEGY).  Set by
 *   set_identification_strategy(), read by get_identification_strategy().
 * - **m_data[0] bit 2** — Cloud flag.  Set when the execution environment is
 *   identified as a cloud provider.
 * - **m_data[0] bits 1-0** — Virtualization summary
 *   (#LCC_API_VIRTUALIZATION_SUMMARY).
 * - **m_data[1..8]** — Proprietary strategy data (8 bytes).  Written by
 *   set_data(), compared by data_match().
 *
 * The total payload size (9 bytes) is defined by
 * #HW_IDENTIFIER_PROPRIETARY_DATA + 1.  The proprietary-data width
 * (#HW_IDENTIFIER_PROPRIETARY_DATA, default 8) is configurable per project
 * in @c projects/&lt;PROJECT&gt;/include/licensecc/&lt;PROJECT&gt;/licensecc_properties.h .
 *
 * The 9-byte payload is serialised to / deserialised from a base64-encoded
 * string via the constructor taking a string and print().
 *
 * @note Strategies #STRATEGY_NONE and #STRATEGY_DEFAULT are not
 *       accepted by set_identification_strategy().
 */
class HwIdentifier {
private:
	std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA + 1> m_data = {};
	friend bool operator==(const HwIdentifier& lhs, const HwIdentifier& rhs);
	friend bool operator!=(const HwIdentifier& lhs, const HwIdentifier& rhs);

public:
	HwIdentifier();

	/**
	 * @brief Construct from a base64-encoded string.
	 * @param param A base64-encoded identifier string (with `-` as line-break
	 *              placeholder).  Must decode to exactly 8 bytes.
	 * @throws std::logic_error if the decoded size is not 8 bytes.
	 */
	explicit HwIdentifier(const std::string& param);

	virtual ~HwIdentifier();

	/** @brief Copy constructor. */
	HwIdentifier(const HwIdentifier& other);

	/**
	 * @brief Set the identification strategy.
	 *
	 * Stores the strategy in bits 5-3 of m_data[0].
	 * @param strategy One of the ::LCC_API_HW_IDENTIFICATION_STRATEGY values.
	 *        #STRATEGY_NONE and #STRATEGY_DEFAULT are rejected.
	 * @throws std::logic_error if strategy is STRATEGY_NONE or STRATEGY_DEFAULT.
	 */
	void set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY strategy);

	/**
	 * @brief Read the identification strategy.
	 * @return The strategy stored in bits 5-3 of m_data[0].
	 */
	LCC_API_HW_IDENTIFICATION_STRATEGY get_identification_strategy() const;

	/**
	 * @brief Set or clear the environment-variable flag.
	 * @param use_env_var If true, sets bit 6 of m_data[0]; otherwise clears it.
	 */
	void set_use_environment_var(bool use_env_var);

	/**
	 * @brief Set or clear the cloud flag.
	 * @param is_cloud If true, sets bit 2 of m_data[0]; otherwise clears it.
	 */
	void set_is_cloud(bool is_cloud);

	/**
	 * @brief Read the cloud flag.
	 * @return true if bit 2 of m_data[0] is set.
	 */
	bool is_cloud() const;

	/**
	 * @brief Set the virtualization summary.
	 *
	 * Stores the value in bits 1-0 of m_data[0].
	 * @param summary One of the ::LCC_API_VIRTUALIZATION_SUMMARY values.
	 */
	void set_virtualization_summary(LCC_API_VIRTUALIZATION_SUMMARY summary);

	/**
	 * @brief Read the virtualization summary.
	 * @return The value stored in bits 1-0 of m_data[0].
	 */
	LCC_API_VIRTUALIZATION_SUMMARY get_virtualization_summary() const;

	/**
	 * @brief Store the 7-byte proprietary data payload.
	 *
	 * Copies data[0..HW_IDENTIFIER_PROPRIETARY_DATA] into m_data[1..HW_IDENTIFIER_PROPRIETARY_DATA+1].
	 * @param data A 7-element array of proprietary strategy data.
	 */
	void set_data(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>& data);

	/**
	 * @brief Compare the stored proprietary data with a given payload.
	 * @param data A 7-element array to compare against m_data[1..7].
	 * @return true if all 7 bytes match exactly.
	 */
	bool data_match(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>& data) const;

	/**
	 * @brief Used to print the identifier for being passed around. Prints in a format XXXX-XXXX-XXXX-XXXX
	 * @return A base64 string with `-` as line-break placeholder.
	 */
	std::string print() const;
	friend std::ostream& operator<<(std::ostream& output, const HwIdentifier& d) {
		output << d.print();
		return output;
	}
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_ */
