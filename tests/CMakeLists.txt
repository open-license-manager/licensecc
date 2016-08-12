
project(metalic_tests)

add_library(metalic_test_licgen STATIC "src/generate.cpp")
target_link_libraries(metalic_test_licgen metalic_licgen)

add_executable(metalic_test_license_generator "src/generator.cpp")
set_target_properties(metalic_test_license_generator PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_license_generator
    metalic_licgen
    ${Boost_LIBRARIES}
)
add_test(metalic_test_license_generator "${CMAKE_BINARY_DIR}/apps/metalic_test_license_generator")

add_executable(metalic_test_standard_license "src/standard_license.cpp")
set_target_properties(metalic_test_standard_license PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_standard_license
    metalic_mlic
    metalic_test_licgen
    ${Boost_LIBRARIES}
)
add_test(metalic_test_standard_license "${CMAKE_BINARY_DIR}/apps/metalic_test_standard_license")

add_executable(metalic_test_date "src/date.cpp")
set_target_properties(metalic_test_date PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_date
    metalic_mlic
    metalic_test_licgen
    ${Boost_LIBRARIES}
)
add_test(metalic_test_date "${CMAKE_BINARY_DIR}/apps/metalic_test_date")

add_executable(metalic_test_volid "src/volid.cpp")
set_target_properties(metalic_test_volid PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_volid
    metalic_mlic
    metalic_test_licgen
    ${Boost_LIBRARIES}
)
add_test(metalic_test_volid "${CMAKE_BINARY_DIR}/apps/metalic_test_volid")

add_executable(metalic_test_license_reader "src/reader.cpp")
set_target_properties(metalic_test_license_reader PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_license_reader
    metalic_mlic
    ${Boost_LIBRARIES}
)
add_test(metalic_test_license_reader "${CMAKE_BINARY_DIR}/apps/metalic_test_license_reader")

if(NOT WIN32)
    add_executable(metalic_test_os_linux "src/linux.cpp")
    set_target_properties(metalic_test_os_linux PROPERTIES FOLDER "tests")
    target_link_libraries(metalic_test_os_linux
        metalic_os
        ${Boost_LIBRARIES}
    )
    add_test(metalic_test_os_linux "${CMAKE_BINARY_DIR}/apps/metalic_test_os_linux")
endif()
