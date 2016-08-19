
project(metalic_tests)

add_executable(metalic_test_license_generator "src/generator.cpp")
set_target_properties(metalic_test_license_generator PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_license_generator
    metalic_generator
    ${Boost_LIBRARIES}
)
add_test(metalic_test_license_generator "${CMAKE_BINARY_DIR}/apps/metalic_test_license_generator")

add_executable(metalic_test_standard_license "src/standard_license.cpp" "src/generate.cpp")
set_target_properties(metalic_test_standard_license PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_standard_license
    metalic_reader
    metalic_generator
    ${Boost_LIBRARIES}
)
add_test(metalic_test_standard_license "${CMAKE_BINARY_DIR}/apps/metalic_test_standard_license")

add_executable(metalic_test_date "src/date.cpp" "src/generate.cpp")
set_target_properties(metalic_test_date PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_date
    metalic_reader
    metalic_generator
    ${Boost_LIBRARIES}
)
add_test(metalic_test_date "${CMAKE_BINARY_DIR}/apps/metalic_test_date")

add_executable(metalic_test_volid "src/volid.cpp" "src/generate.cpp")
set_target_properties(metalic_test_volid PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_volid
    metalic_reader
    metalic_generator
    ${Boost_LIBRARIES}
)
add_test(metalic_test_volid "${CMAKE_BINARY_DIR}/apps/metalic_test_volid")

add_executable(metalic_test_license_reader "src/reader.cpp")
set_target_properties(metalic_test_license_reader PROPERTIES FOLDER "tests")
target_link_libraries(metalic_test_license_reader
    metalic_reader
    ${EXTERNAL_LIBS}
    ${Boost_LIBRARIES}
)
add_test(metalic_test_license_reader "${CMAKE_BINARY_DIR}/apps/metalic_test_license_reader")

if(NOT WIN32)
    add_executable(metalic_test_os_linux
        "src/linux.cpp"
        $<TARGET_OBJECTS:metalic_os>
        $<TARGET_OBJECTS:metalic_base>
        $<TARGET_OBJECTS:metalic_utils>
    )
    include_directories(
        "${CMAKE_SOURCE_DIR}/libs/os/include/"
        "${CMAKE_SOURCE_DIR}/libs/base/include/"
        "${CMAKE_SOURCE_DIR}/libs/utils/include/"
        "${CMAKE_BINARY_DIR}/libs/utils/include/"
    )
    set_target_properties(metalic_test_os_linux PROPERTIES FOLDER "tests")
    target_link_libraries(metalic_test_os_linux ${EXTERNAL_LIBS} ${Boost_LIBRARIES})
    add_test(metalic_test_os_linux "${CMAKE_BINARY_DIR}/apps/metalic_test_os_linux")
endif()
