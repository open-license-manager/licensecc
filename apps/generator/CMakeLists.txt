
project(metalic_generator_app)

add_executable(metalic_generator_app "src/generator.cpp")
target_link_libraries(metalic_generator_app metalic_generator ${EXTERNAL_LIBS})
add_dependencies(metalic_generator_app private_key)
