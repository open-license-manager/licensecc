#if we're here boost has been found
add_definitions(-DBOOST_ALL_NO_LIB) #Disable Boost Microsoft magic, all dependencies are handled by cmake
add_definitions(-DBOOST_LIB_DIAGNOSTIC) #Check it is really disabled
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/../src/templates/licensecc_properties_test.h.in" 
	"${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp"
)

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/licensecc_properties_test.h" 
		INPUT "${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp")
		
add_subdirectory(library)
add_subdirectory(functional)