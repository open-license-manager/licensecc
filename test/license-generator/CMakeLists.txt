
add_executable(
 license_generator_test
 license-generator_test.cpp
)

target_link_libraries(
 license_generator_test
 license_generator_lib
)

SET_TARGET_PROPERTIES(license_generator_test PROPERTIES LINK_SEARCH_START_STATIC ON)
ADD_TEST(license_generator_test ${EXECUTABLE_OUTPUT_PATH}/license_generator_test)
