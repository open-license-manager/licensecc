
project(metalic_os)

add_files(INCLUDE_FILES
    "include/metalicensor/os/os.h"
)
if(WIN32)
    add_library(metalic_os OBJECT
        "src/os-win.cpp"
        ${INCLUDE_FILES}
    )
else()
    add_library(metalic_os OBJECT
        "src/os-linux.cpp"
        "src/network_id.cpp"
        ${INCLUDE_FILES}
    )
endif()
add_dependencies(metalic_os public_key)
target_include_directories(metalic_os
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/base/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/base/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_os PROPERTIES FOLDER "libs")
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
