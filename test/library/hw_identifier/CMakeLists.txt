
add_executable( test_hw_identifier
 hw_identifier_test.cpp
)

target_link_libraries( test_hw_identifier
	licensecc
	${Boost_LIBRARIES}
)

add_executable( test_system_id
  system_id_strategy_test.cpp
)

target_link_libraries( test_system_id
	licensecc
	${Boost_LIBRARIES}
)


if(CODE_COVERAGE AND UNIX)
	target_compile_options(test_hw_identifier PUBLIC -O0 -g --coverage)
	target_link_options(test_hw_identifier PUBLIC --coverage)
	target_link_libraries(test_hw_identifier PUBLIC gcov)
	target_compile_options(test_system_id PUBLIC -O0 -g --coverage)
	target_link_options(test_system_id PUBLIC --coverage)
	target_link_libraries(test_system_id PUBLIC gcov)

endif(CODE_COVERAGE AND UNIX)

ADD_TEST(NAME test_hw_identifier COMMAND test_hw_identifier)
ADD_TEST(NAME test_system_id COMMAND test_system_id)