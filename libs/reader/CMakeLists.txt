
project(metalic_reader)

add_files(INCLUDE_FILES
    "include/metalicensor/reader/LicenseReader.h"
    "include/metalicensor/reader/pc-identifiers.h"
)
add_library(metalic_reader STATIC
    "src/license++.cpp"
    "src/LicenseReader.cpp"
    "src/pc-identifiers.cpp"
    ${INCLUDE_FILES}
    $<TARGET_OBJECTS:metalic_ini>
    $<TARGET_OBJECTS:metalic_os>
    $<TARGET_OBJECTS:metalic_base>
    $<TARGET_OBJECTS:metalic_utils>
)
target_include_directories(metalic_reader
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/ini/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/ini/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/os/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/os/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/base/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/base/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/utils/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/utils/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_reader PROPERTIES FOLDER "libs")
install(TARGETS metalic_reader
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
