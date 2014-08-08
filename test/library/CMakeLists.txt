add_executable(
 license_reader_test
 LicenseReader_test.cpp
)

target_link_libraries(
 license_reader_test
 license++_static
)

add_dependencies( license_reader_test boost_test boost_filesystem )
SET_TARGET_PROPERTIES(license_reader_test PROPERTIES LINK_SEARCH_START_STATIC ON)
ADD_TEST(license_reader_test ${EXECUTABLE_OUTPUT_PATH}/license_reader_test)

#IF(WIN32)
#test windows
#ELSE(WIN32)
    add_executable(
 		os_linux_test
 		Os_Linux_test.cpp
	)

	target_link_libraries(
 		os_linux_test
 		os
	)
	
	add_dependencies( os_linux_test boost_test )
	SET_TARGET_PROPERTIES(os_linux_test PROPERTIES LINK_SEARCH_START_STATIC ON)
	ADD_TEST(os_linux_test ${EXECUTABLE_OUTPUT_PATH}/os_linux_test)   
	
#ENDIF(WIN32)