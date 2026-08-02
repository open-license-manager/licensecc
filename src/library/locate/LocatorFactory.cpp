/*
 * LocatorFactory.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: Gabriele Contini
 */

#include "LocatorStrategy.hpp"
#include "LocatorFactory.hpp"
#include "ApplicationFolder.hpp"
#include "EnvironmentVarData.hpp"
#include "EnvironmentVarLocation.hpp"
#include "ExternalDefinition.hpp"

namespace license {
namespace locate {
std::vector<std::unique_ptr<LocatorStrategy>> LocatorFactory::LocatorFactory::extra_strategies;

bool LocatorFactory::find_license_near_moduleb = FIND_LICENSE_NEAR_MODULE;
bool LocatorFactory::find_license_with_env_varb = FIND_LICENSE_WITH_ENV_VAR;

std::vector<std::unique_ptr<LocatorStrategy>> LocatorFactory::get_active_strategies2(
	const LicenseLocation* locationHint) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	if (find_license_near_moduleb) {
		strategies.push_back(std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ApplicationFolder())));
	}
	if (find_license_with_env_varb) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarLocation())));
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarData())));
	}
	if (locationHint != nullptr) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ExternalDefinition(locationHint))));
	}

	for (auto& strategy : extra_strategies) {
		strategies.push_back(strategy->clone());
	}
	return strategies;
}

void LocatorFactory::set_extra_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies) {
	extra_strategies.clear();
	for (auto& strategy : strategies) {
		extra_strategies.push_back(strategy->clone());
	}
}

FUNCTION_RETURN LocatorFactory::get_active_strategies(std::vector<std::unique_ptr<LocatorStrategy>>& strategies,
													  const LicenseLocation* locationHint) {
	if (find_license_near_moduleb) {
		strategies.push_back(std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ApplicationFolder())));
	}
	if (find_license_with_env_varb) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarLocation())));
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new EnvironmentVarData())));
	}
	if (locationHint != nullptr) {
		strategies.push_back(
			std::unique_ptr<LocatorStrategy>(dynamic_cast<LocatorStrategy*>(new ExternalDefinition(locationHint))));
	}

	for (auto& strategy : extra_strategies) {
		strategies.push_back(strategy->clone());
	}
	return strategies.size() > 0 ? FUNC_RET_OK : FUNC_RET_NOT_AVAIL;
}

LocatorFactory::RawLicenseCursor::RawLicenseCursor(LocatorFactory& factory)
	: parent_factory(factory), current_strategy_idx(0), current_location_idx(0) {
	// Initialize current_locations for the first strategy if available
	if (!parent_factory.active_strategies.empty()) {
		current_locations = parent_factory.active_strategies[0]->license_locations(parent_factory.event_registry);
	}
}

// Prefix increment operator
LocatorFactory::RawLicenseCursor& LocatorFactory::RawLicenseCursor::operator++() {
	current_location_idx++;
	if (current_location_idx >= current_locations.size()) {
		current_location_idx = 0;
		// Move to the next strategy that returns something.
		do {
			current_strategy_idx++;
			if (current_strategy_idx < parent_factory.active_strategies.size()) {
				current_locations = parent_factory.active_strategies[current_strategy_idx]->license_locations(
					parent_factory.event_registry);
			} else {
				current_strategy_idx = parent_factory.active_strategies.size();
			}
		} while (current_strategy_idx < parent_factory.active_strategies.size() && current_locations.empty());
	}
	return *this;
}

// Dereference operator
RawLicenseData LocatorFactory::RawLicenseCursor::operator*() const {
	if (current_strategy_idx < parent_factory.active_strategies.size() &&
		current_location_idx < current_locations.size()) {
		const std ::string cur_loc = current_locations[current_location_idx];
		const std::string data =
			parent_factory.active_strategies[current_strategy_idx]->retrieve_license_content(cur_loc);
		return RawLicenseData(cur_loc, data);
	}
	return RawLicenseData("", "");	// Should not happen in valid iteration
}

// Comparison operator (not equal)
bool LocatorFactory::RawLicenseCursor::operator!=(const RawLicenseCursor& other) const {
	return current_strategy_idx != other.current_strategy_idx || current_location_idx != other.current_location_idx;
}

// Comparison operator (equal)
bool LocatorFactory::RawLicenseCursor::operator==(const RawLicenseCursor& other) const {
	return current_strategy_idx == other.current_strategy_idx && current_location_idx == other.current_location_idx;
}

// Begin method for iterator support
LocatorFactory::RawLicenseCursor LocatorFactory::begin() {
	RawLicenseCursor begin_cur(*this);

	// If there are no strategies, return an end iterator
	EventRegistry& er = begin_cur.parent_factory.event_registry;
	if (begin_cur.parent_factory.active_strategies.empty()) {
		begin_cur.current_strategy_idx = 0;
		er.addEvent(LICENSE_FILE_NOT_FOUND, nullptr);
		er.turnWarningsIntoErrors();
		return begin_cur;
	}

	// If the current strategy has locations, return the current position
	if (!begin_cur.current_locations.empty()) {
		return begin_cur;
	}

	// Otherwise, find the next strategy that has locations using the same logic as increment
	// We need to advance past the current position since it's invalid
	// Use a do-while loop similar to the increment operator
	do {
		begin_cur.current_strategy_idx++;
		if (begin_cur.current_strategy_idx < begin_cur.parent_factory.active_strategies.size()) {
			// Get locations from the next strategy
			begin_cur.current_locations =
				begin_cur.parent_factory.active_strategies[begin_cur.current_strategy_idx]->license_locations(
					begin_cur.parent_factory.event_registry);
		} else {
			begin_cur.current_strategy_idx = begin_cur.parent_factory.active_strategies.size();
			er.addEvent(LICENSE_FILE_NOT_FOUND, nullptr);
			er.turnWarningsIntoErrors();
		}
	} while (begin_cur.current_strategy_idx < begin_cur.parent_factory.active_strategies.size() &&
			 begin_cur.current_locations.empty());

	// At this point, either we found a strategy with locations or we've reached the end
	// If we've reached the end, the cursor is already properly set

	return begin_cur;
}

// End method for iterator support
LocatorFactory::RawLicenseCursor LocatorFactory::end() {
	// Create a cursor that represents the end of iteration. Index of last strategy + 1
	RawLicenseCursor end_cursor(*this);
	end_cursor.current_strategy_idx = active_strategies.size();
	end_cursor.current_location_idx = 0;
	return end_cursor;
}

// Constructor implementation
LocatorFactory::LocatorFactory(const LicenseLocation* locationHint, EventRegistry& eventRegistryRef)
	: event_registry(eventRegistryRef), active_strategies(get_active_strategies2(locationHint)) {}

}  // namespace locate
}  // namespace license
