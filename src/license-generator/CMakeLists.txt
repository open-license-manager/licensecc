
if(WIN32)
ADD_LIBRARY(license_generator_lib STATIC
    license-generator.cpp
    win/LicenseSigner.cpp
)
else()
ADD_LIBRARY(license_generator_lib STATIC
    license-generator.cpp
    linux/LicenseSigner.cpp
)
endif()

link_directories ( ${Boost_LIBRARY_DIRS} )
add_dependencies( license_generator_lib boost_filesystem private_key )

target_link_libraries(
     license_generator_lib
     license++_static
     ${Boost_LIBRARIES}
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

SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)