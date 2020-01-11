#include <array>
#include "identification_strategy.hpp"

namespace license {
using namespace std;
LCC_EVENT_TYPE IdentificationStrategy::validate_identifier(const PcIdentifier& identifier,
														   const vector<array<uint8_t, 6>>& available_ids) const {
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

}  // namespace license
