
project(metalic_generator)

add_executable(metalic_generator "src/generator.cpp")
target_link_libraries(metalic_generator metalic_licgen)
add_dependencies(metalic_generator private_key)
