add_executable( test_license_parser
 LicenseParser_test.cpp
)

target_link_libraries( test_license_parser
   licensecc
   ${Boost_LIBRARIES}
)


### LicenseLocator tests
add_executable(
	test_license_locator_strategies
 	LicenseLocatorStrategies_test.cpp
)

target_link_libraries(
	test_license_locator_strategies
	licensecc
	${Boost_LIBRARIES}
)

### LicenseLocatorFacade tests
add_executable(
	test_license_locator_facade
	LicenseLocatorFacade_test.cpp
)

target_link_libraries(
	test_license_locator_facade
	licensecc
	${Boost_LIBRARIES}
)

### FoundLicenseCursor tests
add_executable(
	test_found_license_cursor
	FoundLicenseCursor_test.cpp
)

target_link_libraries(
	test_found_license_cursor
	licensecc
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
  target_link_libraries(test_license_parser gcov)
  target_link_libraries(test_found_license_cursor gcov)
  target_link_libraries(test_license_locator_facade gcov)
  target_link_libraries(test_license_locator_strategies gcov)
endif(CODE_COVERAGE AND UNIX)

ADD_TEST(NAME test_license_parser COMMAND test_license_parser)
ADD_TEST(NAME test_found_license_cursor COMMAND test_found_license_cursor)
ADD_TEST(NAME test_event_registry COMMAND test_event_registry)

#set environment variables & use static variables, need to run separated from the others
ADD_TEST(NAME test_license_locator_facade COMMAND test_license_locator_facade)
set_tests_properties(test_license_locator_facade PROPERTIES RUN_SERIAL TRUE) 
#set environment variables need to run separated from the others
ADD_TEST(NAME test_license_locator_strategies COMMAND test_license_locator_strategies)
set_tests_properties(test_license_locator_strategies PROPERTIES RUN_SERIAL TRUE) 

ADD_SUBDIRECTORY(os)
ADD_SUBDIRECTORY(hw_identifier)
ADD_SUBDIRECTORY(limits)