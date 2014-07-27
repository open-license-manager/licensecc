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
 license++_static
 license_generator_snippet
)
SET_TARGET_PROPERTIES(standard_license_test PROPERTIES LINK_SEARCH_START_STATIC ON)

add_executable(
 date_test
 date_test.cpp
)
target_link_libraries(
 date_test
 license++_static
 license_generator_snippet
)
SET_TARGET_PROPERTIES(date_test PROPERTIES LINK_SEARCH_START_STATIC ON)

add_executable(
 volid_test
 volid_test.cpp
)
target_link_libraries(
 volid_test
 license++_static
 license_generator_snippet
)

SET_TARGET_PROPERTIES(volid_test PROPERTIES LINK_SEARCH_START_STATIC ON)

ADD_TEST(standard_license_test ${EXECUTABLE_OUTPUT_PATH}/standard_license_test)
ADD_TEST(date_test ${EXECUTABLE_OUTPUT_PATH}/date_test)
ADD_TEST(date_test ${EXECUTABLE_OUTPUT_PATH}/volid_test)
