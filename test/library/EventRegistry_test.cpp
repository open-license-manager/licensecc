#define BOOST_TEST_MODULE "test_event_registry"

#include <iostream>
#include <iterator>

#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/test/unit_test.hpp>
#include <stdlib.h>
#include <cstdio>

#include "../../src/library/base/EventRegistry.h"

namespace test {

using namespace std;
using namespace license;

/**
 * The error reported is for the license that advanced most in the validation process
 *
 */
BOOST_AUTO_TEST_CASE(test_most_advanced_license_error) {
	EventRegistry er;
	er.addEvent(LICENSE_SPECIFIED, "lic2");
	er.addEvent(LICENSE_FOUND, "lic1");
	er.addEvent(LICENSE_CORRUPTED, "lic1");
	er.turnWarningsIntoErrors();
	const AuditEvent* event = er.getLastFailure();
	BOOST_CHECK_MESSAGE(event != nullptr, "An error is detected");
	BOOST_CHECK_MESSAGE(string("lic1") == event->license_reference, "Error is for lic1");
	BOOST_CHECK_MESSAGE(LICENSE_CORRUPTED == event->event_type, "Error is for LICENSE_CORRUPTED");
}

/**
 * Test the new setCurrentLicenseId and overloaded addEvent methods
 */
BOOST_AUTO_TEST_CASE(test_current_license_id_functionality) {
	EventRegistry er;

	// Set the current license ID
	er.setCurrentLicenseId("test_license_123");
	er.addEvent(LICENSE_SPECIFIED);

	BOOST_REQUIRE(er.to_string().find("test_license_123") != string::npos);

	// Get the last event to verify it has the correct license reference
	string eventStr = er.to_string();
	cout << eventStr << endl;
	BOOST_CHECK_MESSAGE(eventStr.find("test_license_123") != string::npos, "Event should contain the set license ID");
	BOOST_CHECK_MESSAGE(eventStr.find("ev:0") != string::npos, "Should have LICENSE_SPECIFIED event (value 0)");
}

BOOST_AUTO_TEST_CASE(test_default_current_license_id_behavior) {
	EventRegistry er;
	er.addEvent(LICENSE_FILE_NOT_FOUND);
	string eventStr = er.to_string();
	BOOST_CHECK_MESSAGE(eventStr.find("UNDEF") != string::npos, "Event should contain UNDEF as default value");
}

}  // namespace test
