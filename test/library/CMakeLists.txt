add_executable( test_license_reader
 LicenseParser_test.cpp
)

target_link_libraries( test_license_reader
   licensecc_static
   ${Boost_LIBRARIES}
)


### LicenseLocator tests
add_executable(
	test_license_locator_strategies
 	LicenseLocatorStrategies_test.cpp
)

target_link_libraries(
	test_license_locator_strategies
	licensecc_static
	${Boost_LIBRARIES}
)

### LicenseLocatorFacade tests
add_executable(
	test_license_locator_facade
	LicenseLocatorFacade_test.cpp
)

target_link_libraries(
	test_license_locator_facade
	licensecc_static
	${Boost_LIBRARIES}
)

### Event Registry tests
add_executable(
 test_event_registry
 EventRegistry_test.cpp
 $<TARGET_OBJECTS:base>
)

target_link_libraries(
 	test_event_registry
 	${Boost_LIBRARIES}
)

if(CODE_COVERAGE AND UNIX)
  target_link_libraries(test_event_registry gcov)
  target_link_libraries(test_license_reader gcov)
endif(CODE_COVERAGE AND UNIX)

ADD_TEST(NAME test_license_reader COMMAND test_license_reader)
ADD_TEST(NAME test_license_locator_strategies COMMAND test_license_locator_strategies)
ADD_TEST(NAME test_license_locator_facade COMMAND test_license_locator_facade)
ADD_TEST(NAME test_event_registry COMMAND test_event_registry)

ADD_SUBDIRECTORY(os)
ADD_SUBDIRECTORY(hw_identifier)