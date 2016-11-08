
project(metalic_ini)

add_files(INCLUDE_FILES
    "include/metalicensor/ini/ConvertUTF.h"
    "include/metalicensor/ini/SimpleIni.h"
)
add_library(metalic_ini OBJECT
    "src/ConvertUTF.cpp"
    ${INCLUDE_FILES}
)
target_include_directories(metalic_ini
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_ini PROPERTIES FOLDER "libs")
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
