ADD_LIBRARY(license_generator_snippet STATIC
 	generate-license.cpp
)

target_link_libraries(
	license_generator_snippet
 	license_generator_lib
)

add_executable(
 standard_license_test
 standard-license_test.cpp
)

target_link_libraries(
 standard_license_test
 licensepp_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)

add_executable(
 date_test
 date_test.cpp
)

target_link_libraries(
 date_test
 licensepp_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)

add_executable(
 volid_test
 volid_test.cpp
)

target_link_libraries(
 volid_test
 licensepp_static
 license_generator_snippet
 ${Boost_LIBRARIES}
)


IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") AND CMAKE_CROSSCOMPILING)
#binfmt_misc doesn't work in my system :(
	ADD_TEST(NAME standard_license_test COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/standard_license_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME date_test COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/date_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME volid_test COMMAND wine ${CMAKE_CURRENT_BINARY_DIR}/volid_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ELSE()
	ADD_TEST(NAME standard_license_test COMMAND standard_license_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME date_test COMMAND date_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	ADD_TEST(NAME volid_test COMMAND volid_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDIF()


