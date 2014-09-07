
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )

if(WIN32)
ADD_LIBRARY(
    license_generator_lib STATIC
    license-generator.cpp
    win/LicenseSigner.cpp
)
else(WIN32)
ADD_LIBRARY(
    license_generator_lib STATIC
    license-generator.cpp
    linux/LicenseSigner.cpp
)
endif(WIN32)

target_link_libraries(
     license_generator_lib
     license++_static
	 $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_DATE_TIME_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_DATE_TIME_LIBRARY_RELEASE}>	 
     ${EXTERNAL_LIBS}
)

add_executable(
 license_generator
 license-generator-main.cpp
)

target_link_libraries(
    license_generator
    license_generator_lib
)
add_dependencies( license_generator private_key )

#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)