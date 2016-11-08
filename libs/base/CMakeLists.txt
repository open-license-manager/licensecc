
project(metalic_base)

add_files(INCLUDE_FILES
    "include/metalicensor/base/base.h"
    "include/metalicensor/base/base64.h"
    "include/metalicensor/base/EventRegistry.h"
    "include/metalicensor/base/logger.h"
    "include/metalicensor/base/StringUtils.h"
)
add_library(metalic_base OBJECT
    "src/EventRegistry.cpp"
    "src/StringUtils.cpp"
    "src/logger.cpp"
    ${INCLUDE_FILES}
)
target_include_directories(metalic_base
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/utils/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/utils/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_base PROPERTIES FOLDER "libs")
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
