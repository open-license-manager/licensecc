add_executable( test_pc_identifier
 pc_identifier_test.cpp
)

target_link_libraries( test_pc_identifier
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

ADD_TEST(NAME test_pc_identifier COMMAND test_pc_identifier)
