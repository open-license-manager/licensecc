
project(metalic_generator)

add_files(INCLUDE_FILES
    "include/metalicensor/generator/license-generator.h"
)
add_library(metalic_generator STATIC
    "src/license-generator.cpp"
    ${INCLUDE_FILES}
)
target_link_libraries(metalic_generator
    metalic_reader
    metalic_crypto
    metalic_utils
    $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>
    $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>
    ${OPENSSL_CRYPTO_LIBRARY}
)
target_include_directories(metalic_generator
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE "$<INSTALL_INTERFACE:include/>"
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
