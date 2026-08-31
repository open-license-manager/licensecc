/*
 * FoundLicenseCursor_test.cpp
 *
 *  Created on: Aug 25, 2026
 *      Author: gab
 */

#define BOOST_TEST_MODULE FoundLicenseCursorTest
#include <boost/test/unit_test.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../../src/library/base/EventRegistry.h"
#include "../../src/library/locate/LocatorFactory.hpp"
#include "../../src/library/locate/LocatorStrategy.hpp"
#include "../../src/library/locate/FoundLicenseCursor.hpp"

namespace test {
using namespace license::locate;
using namespace license;

class StubStrategy : public LocatorStrategy {
private:
	std::vector<std::string> locations_;
	std::string data_;

public:
	StubStrategy(const std::vector<std::string>& locations, const std::string& data)
		: LocatorStrategy("stub"), locations_(locations), data_(data) {}

	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry) override {
		return locations_;
	}

	virtual const std::string retrieve_license_content(const std::string& location) const override { return data_; }

	std::unique_ptr<LocatorStrategy> clone() const override {
		return std::unique_ptr<LocatorStrategy>(new StubStrategy(locations_, data_));
	}
};

BOOST_AUTO_TEST_CASE(IteratesOverMultipleStrategies) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/1"}, "data_1")));
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/2"}, "data_2")));

	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	std::vector<std::string> locations;
	std::vector<std::string> contents;
	int count = 0;
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		RawLicenseData raw = *it;
		locations.push_back(raw.location_id);
		contents.push_back(raw.data);
		if (++count > 4) BOOST_FAIL("cursor missed the end");
	}

	BOOST_CHECK_EQUAL(2, locations.size());
	BOOST_CHECK_EQUAL("/loc/1", locations[0]);
	BOOST_CHECK_EQUAL("/loc/2", locations[1]);
	BOOST_CHECK_EQUAL("data_1", contents[0]);
	BOOST_CHECK_EQUAL("data_2", contents[1]);
}

BOOST_AUTO_TEST_CASE(EmptyStrategiesBeginEqualsEnd) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	BOOST_CHECK(cursor.begin() == cursor.end());
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		BOOST_FAIL("should never enter this for loop.");
	}
}

BOOST_AUTO_TEST_CASE(NoResultStrategiesBeginEqualsEnd) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({}, "unused")));
	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	BOOST_CHECK(cursor.begin() == cursor.end());
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		BOOST_FAIL("should never enter this for loop.");
	}
}

BOOST_AUTO_TEST_CASE(SkipsStrategiesWithoutLocations) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({}, "unused")));
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/2"}, "data_2")));
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({}, "unused")));
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/4"}, "data_4")));

	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	std::vector<std::string> locations;
	int count = 0;
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		RawLicenseData raw = *it;
		locations.push_back(raw.location_id);
		if (++count > 4) BOOST_FAIL("cursor missed the end");
	}

	BOOST_CHECK_EQUAL(2, locations.size());
	BOOST_CHECK_EQUAL("/loc/2", locations[0]);
	BOOST_CHECK_EQUAL("/loc/4", locations[1]);

	for (auto it : cursor) {
		if (++count > 4) BOOST_FAIL("cursor missed the end");
	}
}

BOOST_AUTO_TEST_CASE(DereferenceReturnsLocationAndContent) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/1"}, "license_content")));

	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	auto it = cursor.begin();
	BOOST_CHECK(it != cursor.end());
	RawLicenseData raw = *it;
	BOOST_CHECK_EQUAL("/loc/1", raw.location_id);
	BOOST_CHECK_EQUAL("license_content", raw.data);
}

BOOST_AUTO_TEST_CASE(MultipleLocationsInOneStrategy) {
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	strategies.push_back(
		std::unique_ptr<LocatorStrategy>(new StubStrategy({"/loc/1", "/loc/2", "/loc/3"}, "same_data")));

	EventRegistry eventRegistry;
	FoundLicenseCursor cursor(strategies, eventRegistry);

	std::vector<std::string> locations;
	int count = 0;
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		RawLicenseData raw = *it;
		locations.push_back(raw.location_id);
		BOOST_CHECK_EQUAL("same_data", raw.data);
		if (++count > 5) BOOST_FAIL("cursor missed the end");
	}

	BOOST_CHECK_EQUAL(3, locations.size());
	BOOST_CHECK_EQUAL("/loc/1", locations[0]);
	BOOST_CHECK_EQUAL("/loc/2", locations[1]);
	BOOST_CHECK_EQUAL("/loc/3", locations[2]);
}

}  // namespace test
