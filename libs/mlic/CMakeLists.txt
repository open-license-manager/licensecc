
project(metalic_mlic)

add_files(INCLUDE_FILES
    "include/metalicensor/mlic/LicenseReader.h"
    "include/metalicensor/mlic/pc-identifiers.h"
)
add_library(metalic_mlic STATIC
    "src/license++.cpp"
    "src/LicenseReader.cpp"
    "src/pc-identifiers.c"
    ${INCLUDE_FILES}
)
target_link_libraries(metalic_mlic
    metalic_ini
    metalic_os
)
target_include_directories(metalic_mlic
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_mlic PROPERTIES FOLDER "libs")
install(TARGETS metalic_mlic
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
