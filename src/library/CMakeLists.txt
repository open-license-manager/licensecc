add_subdirectory("base")
add_subdirectory("os")
add_subdirectory("locate")
add_subdirectory("hw_identifier")

set(LICENSECC_SOURCES
    licensecc.cpp
    license_facade.cpp
    LicenseParser.cpp
    limits/license_verifier.cpp
    ini/ConvertUTF.cpp
    $<TARGET_OBJECTS:hw_identifier>
    $<TARGET_OBJECTS:locate>
    $<TARGET_OBJECTS:os>
    $<TARGET_OBJECTS:base>
)

if(BUILD_SHARED_LIBS)
    add_library(licensecc SHARED ${LICENSECC_SOURCES})
    set_target_properties(licensecc PROPERTIES
        VERSION   ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})
    add_library(licensecc::licensecc ALIAS licensecc)

    install(TARGETS licensecc EXPORT licensecc
        ARCHIVE DESTINATION ${main_lib_dest}/${LCC_PROJECT_NAME}
        LIBRARY DESTINATION ${main_lib_dest}/${LCC_PROJECT_NAME}
        RUNTIME DESTINATION bin/${LCC_PROJECT_NAME})
else()
    add_library(licensecc STATIC ${LICENSECC_SOURCES})
    add_library(licensecc::licensecc ALIAS licensecc)

    install(TARGETS licensecc EXPORT licensecc
        ARCHIVE DESTINATION ${main_lib_dest}/${LCC_PROJECT_NAME})
endif()

if(CODE_COVERAGE AND UNIX)
    target_compile_options(licensecc PUBLIC -O0 -g --coverage)
    target_link_options(licensecc PUBLIC --coverage)
    target_link_libraries(licensecc PUBLIC gcov)
endif(CODE_COVERAGE AND UNIX)

define_property(TARGET PROPERTY WITH_OPENSSL BRIEF_DOCS "need openssl to compile" FULL_DOCS "ff")

IF(OPENSSL_FOUND)
    target_compile_definitions(licensecc PUBLIC HAS_OPENSSL)
    set_target_properties(licensecc PROPERTIES WITH_OPENSSL 1)
    target_link_libraries(licensecc PUBLIC OpenSSL::Crypto ${EXTERNAL_LIBS} ${CMAKE_DL_LIBS})
ELSE(OPENSSL_FOUND)
    set_target_properties(licensecc PROPERTIES WITH_OPENSSL 0)
    target_link_libraries(licensecc PUBLIC ${EXTERNAL_LIBS})
ENDIF(OPENSSL_FOUND)

target_include_directories(licensecc PUBLIC
            $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/${LCC_PROJECT_NAME}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
            $<BUILD_INTERFACE:${LCC_INCLUDE_DIR}>)

install(TARGETS lccgen
    RUNTIME DESTINATION bin)
    
install(EXPORT licensecc NAMESPACE licensecc:: DESTINATION  ${main_lib_dest}/${LCC_PROJECT_NAME}/cmake)
export(EXPORT licensecc NAMESPACE licensecc:: FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.cmake)
