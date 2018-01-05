
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )


ADD_LIBRARY(
    license_generator_lib STATIC
    license-generator.cpp
)

target_link_libraries(
     license_generator_lib
     tools_base
     license++_static
	 $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>	 
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

install(TARGETS license_generator RUNTIME DESTINATION bin)

#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)
