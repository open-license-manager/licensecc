
project(metalic_utils)

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/include/metalicensor/build_properties.hpp)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/metalicensor/utils/build_properties.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/metalicensor/utils/build_properties.hpp"
)

add_files(INCLUDE_FILES
    "include/metalicensor/utils/build_properties.hpp"
    "include/metalicensor/api/datatypes.h"
    "include/metalicensor/api/license++.h"
)
add_library(metalic_utils OBJECT
    "src/empty.cpp"
    ${INCLUDE_FILES}
)
target_include_directories(metalic_utils
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_utils PROPERTIES FOLDER "libs")
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/metalicensor/utils/build_properties.hpp"
    DESTINATION "include/metalicensor/utils/"
)
