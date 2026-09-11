/** FoundLicenseCursor.hpp
 *
 *  Created on: Aug 25, 2026
 *      Author: Gabriele Contini
 */

#ifndef SRC_LIBRARY_LOCATE_FOUNDLICENSECURSOR_HPP_
#define SRC_LIBRARY_LOCATE_FOUNDLICENSECURSOR_HPP_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "../base/EventRegistry.h"
#include "LocatorFactory.hpp"
#include <licensecc/LocatorStrategy.hpp>

namespace license {
namespace locate {

/**
 * Cursor that iterates over the raw licenses found by a set of locator strategies.
 * For every location returned by a strategy it dereferences to the RawLicenseData
 * containing the location identifier and the license content. Strategies that do
 * not find any license location are skipped. If no license is found anywhere the
 * cursor logs LICENSE_FILE_NOT_FOUND into the event registry and begin() equals end().
 */
class FoundLicenseCursor {
private:
	const std::vector<std::unique_ptr<LocatorStrategy>>& active_strategies;
	EventRegistry& event_registry;
	size_t current_strategy_idx;
	std::vector<std::string> current_locations;
	size_t current_location_idx;

	void advance_to_next_valid();

public:
	FoundLicenseCursor(const std::vector<std::unique_ptr<LocatorStrategy>>& active_strategies,
					   EventRegistry& eventRegistry);

	FoundLicenseCursor begin();
	FoundLicenseCursor end() const;

	FoundLicenseCursor& operator++();  // Prefix increment
	RawLicenseData operator*() const;  // Dereference operator
	bool operator!=(const FoundLicenseCursor& other) const;	 // Comparison operator
	bool operator==(const FoundLicenseCursor& other) const;	 // Comparison operator
};

}  // namespace locate
}  // namespace license

#endif /* SRC_LIBRARY_LOCATE_FOUNDLICENSECURSOR_HPP_ */
