link_directories ( ${Boost_LIBRARY_DIRS} )

add_executable(
 license_reader_test
 LicenseReader_test.cpp
)

target_link_libraries(
 license_reader_test
 license++_static
 ${Boost_LIBRARIES}
)

SET_TARGET_PROPERTIES(license_reader_test PROPERTIES LINK_SEARCH_START_STATIC ON)
ADD_TEST(license_reader_test ${EXECUTABLE_OUTPUT_PATH}/LicenseReader_test)
