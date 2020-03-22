
ADD_EXECUTABLE(lccinspector inspector.cpp )

target_link_libraries(lccinspector 
     licensecc_static
 	 Boost::unit_test_framework 
     Boost::filesystem
     Boost::system)