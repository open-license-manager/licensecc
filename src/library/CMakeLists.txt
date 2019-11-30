add_subdirectory("os")
add_subdirectory("base")
add_subdirectory("locate")


ADD_LIBRARY(licensecc_static STATIC
    licensecc.cpp
    LicenseReader.cpp
    pc-identifiers.c
    limits/license_verifier.cpp
    ini/ConvertUTF.c
)

target_link_libraries(
     licensecc_static
     locators
     os
     base
)

target_include_directories(licensecc_static
    INTERFACE 
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS licensecc_static ARCHIVE DESTINATION lib/${PROJECT_NAME}/${LCC_PROJECT_NAME})

