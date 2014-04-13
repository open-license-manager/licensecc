#if we're here boost has been found
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIRS} )

add_subdirectory(library)
add_subdirectory(license-generator)
add_subdirectory(functional)