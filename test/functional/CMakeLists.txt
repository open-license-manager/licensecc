
add_executable(
 standard_license_test
 standard-license_test.cpp
)

target_link_libraries(
 standard_license_test
 license++_static
 license_generator_lib
)

SET_TARGET_PROPERTIES(standard_license_test PROPERTIES LINK_SEARCH_START_STATIC ON)
ADD_TEST(standard_license_test ${EXECUTABLE_OUTPUT_PATH}/standard_license_test)
