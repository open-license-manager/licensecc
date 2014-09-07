#if we're here boost has been found
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )

add_subdirectory(library)
add_subdirectory(license-generator)
add_subdirectory(functional)