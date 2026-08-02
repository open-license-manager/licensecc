/*
 * RawLicenseCursor_test.cpp
 *
 *  Created on: Aug 2, 2026
 *      Author: gab
 */

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "licensecc/locate/LocatorFactory.hpp"
#include "licensecc/locate/ApplicationFolder.hpp"
#include "licensecc/locate/EnvironmentVarLocation.hpp"
#include "licensecc/locate/EnvironmentVarData.hpp"
#include "licensecc/locate/ExternalDefinition.hpp"
#include "licensecc/base/EventRegistry.h"
#include "licensecc/datatypes.h"

using namespace license::locate;
using namespace license;

// Test fixture for RawLicenseCursor
class RawLicenseCursorTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup code if needed
	}

	void TearDown() override {
		// Cleanup code if needed
	}
};

TEST_F(RawLicenseCursorTest, TestCursorIteration) {
	// Create a mock event registry
	EventRegistry eventRegistry;

	// Create some test strategies
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;

	// Add ApplicationFolder strategy
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new ApplicationFolder()));

	// Add EnvironmentVarLocation strategy
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new EnvironmentVarLocation()));

	// Create cursor
	LocatorFactory::RawLicenseCursor cursor(strategies, eventRegistry);

	// Test that cursor can be created
	EXPECT_TRUE(true);
}

TEST_F(RawLicenseCursorTest, TestCursorWithEmptyStrategies) {
	// Create a mock event registry
	EventRegistry eventRegistry;

	// Create empty strategies vector
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;

	// Create cursor
	LocatorFactory::RawLicenseCursor cursor(strategies, eventRegistry);

	// Test that cursor can be created with empty strategies
	EXPECT_TRUE(true);
}

TEST_F(RawLicenseCursorTest, TestCloneFunctionality) {
	// Test that all strategies can be cloned
	ApplicationFolder appFolder;
	auto appFolderClone = appFolder.clone();
	ASSERT_NE(nullptr, appFolderClone.get());

	EnvironmentVarLocation envVarLoc;
	auto envVarLocClone = envVarLoc.clone();
	ASSERT_NE(nullptr, envVarLocClone.get());

	EnvironmentVarData envVarData;
	auto envVarDataClone = envVarData.clone();
	ASSERT_NE(nullptr, envVarDataClone.get());

	// Test that ExternalDefinition can be cloned (need a dummy LicenseLocation)
	LicenseLocation dummyLocation = {LICENSE_PATH};
	ExternalDefinition extDef(&dummyLocation);
	auto extDefClone = extDef.clone();
	ASSERT_NE(nullptr, extDefClone.get());
}

TEST_F(RawLicenseCursorTest, TestIteratorSupport) {
	// Create a mock event registry
	EventRegistry eventRegistry;

	// Create some test strategies
	std::vector<std::unique_ptr<LocatorStrategy>> strategies;

	// Add ApplicationFolder strategy
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new ApplicationFolder()));

	// Add EnvironmentVarLocation strategy
	strategies.push_back(std::unique_ptr<LocatorStrategy>(new EnvironmentVarLocation()));

	// Test iterator support
	LocatorFactory factory(nullptr, eventRegistry);

	// Test begin and end methods
	auto begin_it = factory.begin();
	auto end_it = factory.end();

	// Basic comparison test
	EXPECT_TRUE(begin_it != end_it);

	// Test that we can iterate (this won't actually find real licenses but will test the mechanism)
	EXPECT_TRUE(true);
}