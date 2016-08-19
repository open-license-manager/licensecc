
project(metalic_generator)

add_files(INCLUDE_FILES
    "include/metalicensor/generator/license-generator.h"
)
add_library(metalic_generator SHARED
    "src/license-generator.cpp"
    ${INCLUDE_FILES}
    $<TARGET_OBJECTS:metalic_crypto>
    $<TARGET_OBJECTS:metalic_utils>
)
target_link_libraries(metalic_generator
    metalic_reader
    $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>
    $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>
    ${EXTERNAL_LIBS}
)
target_include_directories(metalic_generator
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/crypto/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/crypto/include/>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libs/utils/include/>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/utils/include/>"
    INTERFACE
        "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_generator PROPERTIES FOLDER "libs")
install(TARGETS metalic_generator
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
