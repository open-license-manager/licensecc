ADD_LIBRARY(license_generator_snippet STATIC
 	generate-license.cpp
)

target_link_libraries(
	license_generator_snippet
 	license_generator_lib
)

add_executable(
 test_standard_license
 standard-license_test.cpp
)

target_link_libraries(
 test_standard_license
 licensecc_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)

add_executable(
 test_date
 date_test.cpp
)

target_link_libraries(
 test_date
 licensecc_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)

add_executable(
 test_volid
 volid_test.cpp
)

target_link_libraries(
 test_volid
 licensecc_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)

#set CROSSCOMPILING_EMULATOR
IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") AND CMAKE_CROSSCOMPILING)
#binfmt_misc doesn't work in my system :(
	ADD_TEST(NAME test_standard_license COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/test_standard_license WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME test_date COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/test_date WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME test_volid COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/test_volid WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ELSE()
	ADD_TEST(NAME test_standard_license COMMAND test_standard_license WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME test_date COMMAND test_date WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME test_volid COMMAND test_volid WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDIF()


