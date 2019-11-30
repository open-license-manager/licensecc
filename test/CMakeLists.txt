#if we're here boost has been found
add_definitions(-DBOOST_ALL_NO_LIB) #Disable Boost Microsoft magic, all dependencies are handled by cmake
add_definitions(-DBOOST_LIB_DIAGNOSTIC) #Check it is really disabled
include_directories(${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/../src/templates/licensecc_properties_test.h.in" 
	"${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp"
)
IF(CMAKE_VERSION VERSION_GREATER_EQUAL 3.0)
	file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include/$<CONFIG>/licensecc_properties_test.h" 
		INPUT "${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp")
	include_directories ( ${CMAKE_BINARY_DIR}/include/$<CONFIG> )
ELSE(CMAKE_VERSION VERSION_GREATER_EQUAL 3.0)
#patch for Centos 7 cmake 2.8.11 :(
	file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include/$<CONFIGURATION>/licensecc_properties_test.h" 
		INPUT "${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp")
	include_directories ( ${CMAKE_BINARY_DIR}/include/$<CONFIGURATION> )
endif(CMAKE_VERSION VERSION_GREATER_EQUAL 3.0)

add_subdirectory(library)
add_subdirectory(functional)