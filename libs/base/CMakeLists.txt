
project(metalic_base)

add_files(INCLUDE_FILES
    "include/metalicensor/base/base.h"
    "include/metalicensor/base/base64.h"
    "include/metalicensor/base/EventRegistry.h"
    "include/metalicensor/base/logger.h"
    "include/metalicensor/base/StringUtils.h"
)
add_library(metalic_base STATIC
    "src/EventRegistry.cpp"
    "src/StringUtils.cpp"
    "src/logger.c"
    ${INCLUDE_FILES}
)
target_link_libraries(metalic_base metalic_utils)
#add_dependencies(metalic_base public_key)
target_include_directories(metalic_base
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_base PROPERTIES FOLDER "libs")
install(TARGETS metalic_base
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
