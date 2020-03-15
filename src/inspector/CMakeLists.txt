
ADD_EXECUTABLE(inspector inspector.cpp )

target_link_libraries(inspector 
     licensecc_static
 	 Boost::unit_test_framework 
     Boost::filesystem
     Boost::system)