/*
 * pc_identifier.h
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
namespace pc_identifier {

#define PC_IDENTIFIER_PROPRIETARY_DATA 8

/**
 * data[0]
 * bit 7 = 0 if pc id is being generated 1 if it is coming from an issued license.
 *
 * if bit 7 = 0
 * bit 6 = environment variable was used to generate pc_id
 * bit 5-4 = execution environment information (0=BARE_TO_METAL,1=VM,2=CONTAINER)
 * bit 3 = 0 on premise vm 1 = cloud
 * if on premise vm bits 2-1-0 are virtualization technology
 * if cloud vm bits 2-1-0 identify cloud provider
 *
 * if bit 7 = 1 pc identifier is used to enable some flag that we don't want to show openly in the license
 * bit 6 = 1 enable magic file/registry key
 * ----
 * data[1] bit 7-6-5 define identification strategy.
 * data[1] bits 4-0, data[2-8] are pc identifier proprietary strategy data.
 */

class PcIdentifier {
private:
	std::array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA + 1> m_data = {};

public:
	PcIdentifier();
	PcIdentifier(const std::string &param);
	virtual ~PcIdentifier();
	PcIdentifier(const PcIdentifier &other);
	void set_identification_strategy(LCC_API_IDENTIFICATION_STRATEGY strategy);
	LCC_API_IDENTIFICATION_STRATEGY get_identification_strategy() const;
	void set_use_environment_var(bool use_env_var);
	void set_virtual_environment(VIRTUALIZATION virtualization);
	void set_virtualization(VIRTUALIZATION_DETAIL virtualization_detail);
	void set_cloud_provider(CLOUD_PROVIDER cloud_provider);
	void set_data(const std::array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA> &data);
	bool data_match(const std::array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA> &data) const;
	std::string print() const;
	friend std::ostream &operator<<(std::ostream &output, const PcIdentifier &d) {
		output << d.print();
		return output;
	};
};

}  // namespace pc_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_ */
