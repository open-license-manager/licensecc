add_executable(
 license_reader_test
 LicenseReader_test.cpp
)

target_link_libraries(
 license_reader_test
 licensecc_static
 ${Boost_LIBRARIES}
)

IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") AND CMAKE_CROSSCOMPILING)
#binfmt_misc doesn't work in my system :(
	ADD_TEST(NAME license_reader_test COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/license_reader_test)
ELSE()
	ADD_TEST(NAME license_reader_test COMMAND license_reader_test)
ENDIF()

IF(WIN32)
#test windows
ELSE(WIN32)
    add_executable(
 		os_linux_test
 		Os_Linux_test.cpp
	)

	target_link_libraries(
 		os_linux_test
 		os
 		${Boost_LIBRARIES}
	)

	ADD_TEST(NAME os_linux_test COMMAND os_linux_test)
ENDIF(WIN32)


### LicenseLocator tests
add_executable(
 license_locator_test
 LicenseLocator_test.cpp
)

target_link_libraries(
 license_locator_test
 locators
 ${Boost_LIBRARIES}
)

IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") AND CMAKE_CROSSCOMPILING)
#binfmt_misc doesn't work in my system :(
	ADD_TEST(NAME license_locator_test COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/license_locator_test)
ELSE()
	ADD_TEST(NAME license_locator_test COMMAND license_locator_test)
ENDIF()