add_executable(
 license_reader_test
 LicenseReader_test.cpp
)
#add_dependencies( license_reader_test boost_filesystem )

target_link_libraries(
 license_reader_test
 license++_static
 ${Boost_LIBRARIES}
)

#SET_TARGET_PROPERTIES(license_reader_test PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_reader_test PROPERTIES LINK_SEARCH_END_STATIC OFF)
ADD_TEST(NAME license_reader_test COMMAND license_reader_test)

IF(WIN32)
#test windows
ELSE(WIN32)
    add_executable(
 		os_linux_test
 		Os_Linux_test.cpp
	)

	target_link_libraries(
 		os_linux_test
 		os
 		${Boost_LIBRARIES}
	)
	
	#add_dependencies( os_linux_test )
	#SET_TARGET_PROPERTIES(os_linux_test PROPERTIES LINK_SEARCH_START_STATIC ON)
	ADD_TEST(NAME os_linux_test COMMAND os_linux_test)
	
ENDIF(WIN32)
