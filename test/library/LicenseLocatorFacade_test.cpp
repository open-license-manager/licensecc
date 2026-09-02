/*
 * LicenseLocatorFacade_test.cpp
 *
 *  Created on: Aug 2, 2026
 *      Author: gab
 */

#define BOOST_TEST_MODULE LicenseLocatorFacadeTest
#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include "../../src/library/os/os.h"
#include <licensecc/EventRegistry.h>
#include "../../src/library/locate/ApplicationFolder.hpp"
#include "../../src/library/locate/EnvironmentVarLocation.hpp"
#include "../../src/library/locate/ExternalDefinition.hpp"
#include "licensecc/datatypes.h"
#include "../../src/library/locate/LocatorFactory.hpp"
#include <licensecc/LocatorStrategy.hpp>
#include "../../src/library/locate/FoundLicenseCursor.hpp"
namespace test {
using namespace license::locate;
using namespace license;

// Custom test strategy that returns static strings
class TestLocatorStrategy : public LocatorStrategy {
private:
	std::string location_;
	std::string data_;

public:
	TestLocatorStrategy(const std::string& location, const std::string& data)
		: LocatorStrategy("test"), location_(location), data_(data) {}

	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry) override {
		return {location_};
	}

	virtual const std::string retrieve_license_content(const std::string& location) const override { return data_; }

	std::unique_ptr<LocatorStrategy> clone() const override {
		return std::unique_ptr<LocatorStrategy>(new TestLocatorStrategy(location_, data_));
	}
};

class TestLocatorEmptyStrategy : public LocatorStrategy {
public:
	TestLocatorEmptyStrategy() : LocatorStrategy("test_empty") {}

	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry) override {
		return std::vector<std::string>();
	}

	virtual const std::string retrieve_license_content(const std::string& location) const override { return ""; }

	std::unique_ptr<LocatorStrategy> clone() const override {
		return std::unique_ptr<LocatorStrategy>(new TestLocatorEmptyStrategy());
	}
};

BOOST_AUTO_TEST_CASE(TestNoStrategy) {
	// Set up the static methods
	LocatorFactory::find_license_near_module(false);
	LocatorFactory::find_license_with_env_var(false);
	std::vector<std::unique_ptr<LocatorStrategy>> extra_strategies;
	LocatorFactory::set_extra_strategies(extra_strategies);

	EventRegistry eventRegistry;
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	const FUNCTION_RETURN ret = LocatorFactory::get_active_strategies(strategies, nullptr);
	BOOST_CHECK_EQUAL(FUNC_RET_NOT_AVAIL, ret);
	BOOST_CHECK_EQUAL(strategies.size(), 0);
}

// if there is a registered strategy but this does not return any location, it shoud not loop
BOOST_AUTO_TEST_CASE(TestEmptyStrategy) {
	LocatorFactory::find_license_near_module(false);
	LocatorFactory::find_license_with_env_var(false);
	std::vector<std::unique_ptr<LocatorStrategy>> extra_strategies;
	extra_strategies.push_back(std::unique_ptr<LocatorStrategy>(new TestLocatorEmptyStrategy()));
	LocatorFactory::set_extra_strategies(extra_strategies);

	EventRegistry eventRegistry;
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	const FUNCTION_RETURN ret = LocatorFactory::get_active_strategies(strategies, nullptr);
	BOOST_CHECK_EQUAL(FUNC_RET_OK, ret);
	BOOST_CHECK_EQUAL(strategies.size(), 1);

	FoundLicenseCursor cursor(strategies, eventRegistry);
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		BOOST_FAIL("should never enter this for loop.");
	}
	eventRegistry.turnWarningsIntoErrors();
	BOOST_CHECK(!eventRegistry.isGood());
	BOOST_ASSERT(eventRegistry.getLastFailure() != NULL);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND, eventRegistry.getLastFailure()->event_type);
}

BOOST_AUTO_TEST_CASE(TestCustomStrategy) {
	LocatorFactory::find_license_near_module(false);
	LocatorFactory::find_license_with_env_var(false);
	std::vector<std::unique_ptr<LocatorStrategy>> extra_strategies;
	extra_strategies.push_back(std::unique_ptr<LocatorStrategy>(new TestLocatorEmptyStrategy()));
	extra_strategies.push_back(
		std::unique_ptr<LocatorStrategy>(new TestLocatorStrategy("/test/location1", "license_data_1")));
	extra_strategies.push_back(
		std::unique_ptr<LocatorStrategy>(new TestLocatorStrategy("/test/location2", "license_data_2")));
	LocatorFactory::set_extra_strategies(extra_strategies);

	EventRegistry eventRegistry;
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	const FUNCTION_RETURN ret = LocatorFactory::get_active_strategies(strategies, nullptr);
	BOOST_CHECK_EQUAL(FUNC_RET_OK, ret);
	BOOST_CHECK_EQUAL(strategies.size(), 3);

	FoundLicenseCursor cursor(strategies, eventRegistry);
	// Iterate over the cursor to check that two licenses are returned
	std::vector<std::string> returned_data;
	int license_count = 0;
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		RawLicenseData license_data = *it;
		returned_data.push_back(license_data.data);
		license_count++;
		// in case we miss the end.
		if (license_count > 3) break;
	}

	// Should find two licenses
	BOOST_CHECK_EQUAL(returned_data.size(), 2);
	BOOST_CHECK_EQUAL(returned_data[0], "license_data_1");
	BOOST_CHECK_EQUAL(returned_data[1], "license_data_2");
	BOOST_CHECK(eventRegistry.isGood());
}

BOOST_AUTO_TEST_CASE(TestEnvVarStrategy) {
	// activates 2 strategies at the same time: EnvironmentVarLocation, EnvironmentVarData
	LocatorFactory::find_license_with_env_var(true);
	LocatorFactory::find_license_near_module(false);
	std::vector<std::unique_ptr<LocatorStrategy>> extra_strategies;
	LocatorFactory::set_extra_strategies(extra_strategies);

	// Set environment variable with base64 encoded license data
	const char* env_var_name = LCC_LICENSE_DATA_ENV_VAR;
	const char* env_var_value = "SGVsbG8gV29ybGQ=";	 // "Hello World" in base64
	SETENV(env_var_name, env_var_value);

	EventRegistry eventRegistry;
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;
	const FUNCTION_RETURN ret = LocatorFactory::get_active_strategies(strategies, nullptr);
	BOOST_CHECK_EQUAL(FUNC_RET_OK, ret);
	// EnvironmentVarLocation, EnvironmentVarData
	BOOST_CHECK_EQUAL(strategies.size(), 2);

	FoundLicenseCursor cursor(strategies, eventRegistry);
	int license_count = 0;
	std::string returned_data;
	for (auto it = cursor.begin(); it != cursor.end(); ++it) {
		auto license_data = *it;
		returned_data = license_data.data;
		license_count++;
		if (license_count > 1) break;
	}

	BOOST_CHECK_EQUAL(license_count, 1);
	BOOST_CHECK(eventRegistry.isGood());
	BOOST_CHECK_EQUAL(returned_data, "Hello World");

	UNSETENV(env_var_name);
}

}  // namespace test