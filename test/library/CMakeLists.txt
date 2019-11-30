add_executable(
 test_license_reader
 LicenseReader_test.cpp
)

target_link_libraries(
 test_license_reader
 licensecc_static
 ${Boost_LIBRARIES}
)

ADD_TEST(NAME test_license_reader COMMAND test_license_reader)

IF(WIN32)
#test windows
ELSE(WIN32)
    add_executable(
 		test_os_linux
 		Os_Linux_test.cpp
	)

	target_link_libraries(
 		test_os_linux
 		os
 		${Boost_LIBRARIES}
	)

	ADD_TEST(NAME test_os_linux COMMAND test_os_linux)
ENDIF(WIN32)

### LicenseLocator tests
add_executable(
 test_license_locator
 LicenseLocator_test.cpp
)

target_link_libraries(
 test_license_locator
 locators
 ${Boost_LIBRARIES}
)

ADD_TEST(NAME test_license_locator COMMAND test_license_locator)

### LicenseLocator tests
add_executable(
 test_event_registry
 EventRegistry_test.cpp
)

target_link_libraries(
 test_event_registry
 base
 ${Boost_LIBRARIES}
)

ADD_TEST(NAME test_event_registry COMMAND test_event_registry)
