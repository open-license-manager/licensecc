/*
 * ethernet.hpp
 *
 *  Created on: Jan 11, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_

#include "identification_strategy.hpp"

namespace license {
namespace pc_identifier {

class Ethernet : public IdentificationStrategy {
private:
	const bool use_ip;
public:
	Ethernet(bool use_ip);
	virtual ~Ethernet();
	virtual LCC_API_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual FUNCTION_RETURN identify_pc(PcIdentifier &pc_id) const;
	virtual std::vector<PcIdentifier> alternative_ids() const;
	using IdentificationStrategy::validate_identifier;
	virtual LCC_EVENT_TYPE validate_identifier(const PcIdentifier &identifier) const;
};

}  // namespace pc_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_ */
