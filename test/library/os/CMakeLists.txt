add_executable( test_network
 network_test.cpp
)

target_link_libraries( test_network
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

ADD_TEST(NAME test_network COMMAND test_network)

if(CODE_COVERAGE AND UNIX)
	target_compile_options(test_network PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)
