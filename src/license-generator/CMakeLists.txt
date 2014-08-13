
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

#link_directories ( ${Boost_LIBRARY_DIRS} )
#add_dependencies( license_generator_lib boost_filesystem private_key )

target_link_libraries(
     license_generator_lib
     license++_static
     ${Boost_filesystem_LIBRARY}
     ${Boost_program_options_LIBRARY}
     ${Boost_system_LIBRARY}
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

#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)