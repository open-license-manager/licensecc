
project(metalic_bootstrap)

add_executable(metalic_bootstrap
    "src/bootstrap.cpp"
    $<TARGET_OBJECTS:metalic_crypto>
)
include_directories(${CMAKE_SOURCE_DIR}/libs/crypto/include/)
target_link_libraries(metalic_bootstrap ${EXTERNAL_LIBS})
set_target_properties(metalic_bootstrap PROPERTIES FOLDER "apps")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/keys/metalicensor/keys/)
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/private-key.h" "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/public-key.h"
  COMMAND metalic_bootstrap "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/private-key.h" "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/public-key.h"
  DEPENDS metalic_bootstrap
)
add_custom_target(private_key DEPENDS "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/private-key.h")
add_custom_target(public_key DEPENDS "${CMAKE_BINARY_DIR}/keys/metalicensor/keys/public-key.h")
install(DIRECTORY "${CMAKE_BINARY_DIR}/keys/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
