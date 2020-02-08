#include <array>
#include "identification_strategy.hpp"
#include "default_strategy.hpp"
#include "ethernet.hpp"
#include "disk_strategy.hpp"
namespace license {
namespace pc_identifier {

using namespace std;
LCC_EVENT_TYPE IdentificationStrategy::validate_identifier(
	const PcIdentifier& identifier, const vector<array<uint8_t, PC_IDENTIFIER_PROPRIETARY_DATA>>& available_ids) const {
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;

	if (identifier.get_identification_strategy() == identification_strategy()) {
		for (auto& it : available_ids) {
			if (identifier.data_match(it)) {
				result = LICENSE_OK;
				break;
			}
		}
	}
	return result;
}

std::unique_ptr<IdentificationStrategy> IdentificationStrategy::get_strategy(LCC_API_IDENTIFICATION_STRATEGY strategy) {
	unique_ptr<IdentificationStrategy> result;
	switch (strategy) {
		case STRATEGY_DEFAULT:
			result = unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new DefaultStrategy()));
			break;
		case STRATEGY_ETHERNET:
			result = unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new Ethernet(false)));
			break;
		case STRATEGY_IP_ADDRESS:
			result = unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new Ethernet(true)));
			break;
		case STRATEGY_DISK_NUM:
			result = unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new DiskStrategy(true)));
			break;
		case STRATEGY_DISK_LABEL:
			result = unique_ptr<IdentificationStrategy>(dynamic_cast<IdentificationStrategy*>(new DiskStrategy(false)));
			break;
		default:
			throw logic_error("strategy not supported");
	}
	return result;
}
}  // namespace pc_identifier
}  // namespace license

