
project(metalic_os)

add_files(INCLUDE_FILES
    "include/metalicensor/os/os.h"
)
if(WIN32)
    add_library(metalic_os STATIC
        "src/os-win.c"
        ${INCLUDE_FILES}
    )
else()
    add_library(metalic_os STATIC
        "src/os-linux.c"
        "src/network_id.c"
        ${INCLUDE_FILES}
    )
endif()
target_link_libraries(metalic_os
     metalic_base
     ${EXTERNAL_LIBS}
)
add_dependencies(metalic_os public_key)
target_include_directories(metalic_os
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_os PROPERTIES FOLDER "libs")
install(TARGETS metalic_os
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
