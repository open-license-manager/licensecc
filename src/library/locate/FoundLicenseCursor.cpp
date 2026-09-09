/*
 * FoundLicenseCursor.cpp
 *
 *  Created on: Aug 25, 2026
 *      Author: Gabriele Contini
 */

#include <string>
#include <vector>

#include "FoundLicenseCursor.hpp"

namespace license {
namespace locate {

FoundLicenseCursor::FoundLicenseCursor(const std::vector<std::unique_ptr<LocatorStrategy>>& active_strategies,
									   EventRegistry& eventRegistry)
	: active_strategies(active_strategies),
	  event_registry(eventRegistry),
	  current_strategy_idx(0),
	  current_location_idx(0) {}

void FoundLicenseCursor::advance_to_next_valid() {
	while (current_strategy_idx < active_strategies.size() && current_locations.empty()) {
		const std::string strategy_name = active_strategies[current_strategy_idx]->get_strategy_name();
		const LCC_EVENT_TYPE result = active_strategies[current_strategy_idx]->license_locations(current_locations);
		if (current_locations.empty()) {
			event_registry.addEvent(result, strategy_name.c_str());
			current_strategy_idx++;
			current_location_idx = 0;
		} else {
			event_registry.addEvent(LICENSE_SPECIFIED, strategy_name.c_str());
		}
	}
}

FoundLicenseCursor FoundLicenseCursor::begin() {
	FoundLicenseCursor begin_cursor(active_strategies, event_registry);
	begin_cursor.advance_to_next_valid();
	if (begin_cursor.current_strategy_idx == active_strategies.size() && active_strategies.empty()) {
		event_registry.addEvent(LICENSE_FILE_NOT_FOUND);
	}
	return begin_cursor;
}

FoundLicenseCursor FoundLicenseCursor::end() const {
	FoundLicenseCursor end_cursor(active_strategies, event_registry);
	end_cursor.current_strategy_idx = active_strategies.size();
	end_cursor.current_location_idx = 0;
	return end_cursor;
}

FoundLicenseCursor& FoundLicenseCursor::operator++() {
	current_location_idx++;
	if (current_location_idx >= current_locations.size()) {
		current_location_idx = 0;
		current_locations.clear();
		current_strategy_idx++;
		advance_to_next_valid();
	}
	return *this;
}

RawLicenseData FoundLicenseCursor::operator*() const {
	if (current_strategy_idx < active_strategies.size() && current_location_idx < current_locations.size()) {
		const std::string cur_loc = current_locations[current_location_idx];
		std::string data;
		const LCC_EVENT_TYPE ret = active_strategies[current_strategy_idx]->retrieve_license_content(cur_loc, data);
		event_registry.setCurrentLicenseId(cur_loc);
		if (ret == LICENSE_FOUND) {
			event_registry.addEvent(LICENSE_FOUND, cur_loc);
			return RawLicenseData(cur_loc, data);
		}
		event_registry.addEvent(LICENSE_FILE_NOT_FOUND, cur_loc);
	}
	return RawLicenseData("", "");
}

bool FoundLicenseCursor::operator!=(const FoundLicenseCursor& other) const {
	return current_strategy_idx != other.current_strategy_idx || current_location_idx != other.current_location_idx;
}

bool FoundLicenseCursor::operator==(const FoundLicenseCursor& other) const {
	return current_strategy_idx == other.current_strategy_idx && current_location_idx == other.current_location_idx;
}

}  // namespace locate
}  // namespace license
