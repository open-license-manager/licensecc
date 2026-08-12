add_executable( test_network
 network_test.cpp
 "network_test.cpp")

target_link_libraries( test_network 
	licensecc_static ${Boost_LIBRARIES})

ADD_TEST(NAME test_network COMMAND test_network)

IF(NOT DEFINED MINGW)
	add_executable( test_board_info board_info_test.cpp "board_info_test.cpp")
	target_link_libraries( test_board_info
 		licensecc_static
		${Boost_LIBRARIES}
	)
	ADD_TEST(NAME test_board_info COMMAND test_board_info)

ENDIF(NOT DEFINED MINGW)

add_executable( test_cpu_info cpu_info_test.cpp "cpu_info_test.cpp")
	target_link_libraries( test_cpu_info
 		licensecc_static
		${Boost_LIBRARIES}
	)
ADD_TEST(NAME test_cpu_info COMMAND test_cpu_info)

add_executable( test_execution_environment
 	execution_environment_test.cpp
)

target_link_libraries( test_execution_environment
 	licensecc_static
	${Boost_LIBRARIES}
)

ADD_TEST(NAME test_execution_environment COMMAND test_execution_environment)

IF(WIN32)
#test windows
ELSE(WIN32)
    add_executable(
 		test_os_linux
 		os_linux_test.cpp
	)

	target_link_libraries(
 		test_os_linux
 		licensecc_static
		${Boost_LIBRARIES}
	)

	ADD_TEST(NAME test_os_linux COMMAND test_os_linux)
ENDIF(WIN32)

#check if still needed, maybe exported by the base lib
if(CODE_COVERAGE AND UNIX)
	IF(NOT DEFINED MINGW)
		target_compile_options(test_dmi_info PUBLIC -O0 -g --coverage)
	ENDIF(NOT DEFINED MINGW)
	
	target_compile_options(test_execution_environment PUBLIC -O0 -g --coverage)
	target_compile_options(test_network PUBLIC -O0 -g --coverage)
	target_compile_options(test_os_linux PUBLIC -O0 -g --coverage)

endif(CODE_COVERAGE AND UNIX)

