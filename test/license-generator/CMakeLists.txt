
add_executable(
 license_generator_test
 license-generator_test.cpp
)

target_link_libraries(
 license_generator_test
 license_generator_lib
  ${Boost_LIBRARIES}
)

#add_dependencies( license_generator_test boost_test boost_filesystem )
#SET_TARGET_PROPERTIES(license_generator_test PROPERTIES LINK_SEARCH_START_STATIC ON)
ADD_TEST(NAME license_generator_test COMMAND license_generator_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
