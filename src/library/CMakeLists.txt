add_subdirectory("base")

ADD_LIBRARY(licensecc_static STATIC
    licensecc.cpp
    LicenseReader.cpp
    pc-identifiers.c
    limits/license_verifier.cpp
    ini/ConvertUTF.c
)

add_subdirectory("locate")
add_subdirectory("os")

define_property(TARGET PROPERTY WITH_OPENSSL BRIEF_DOCS "need openssl to compile" FULL_DOCS "ff")
IF(UNIX OR OPENSSL_FOUND)
	target_compile_definitions(licensecc_static PUBLIC HAS_OPENSSL)
	set_target_properties(licensecc_static PROPERTIES WITH_OPENSSL 1)
	target_link_libraries(licensecc_static PUBLIC base OpenSSL::Crypto ${EXTERNAL_LIBS} ${CMAKE_DL_LIBS})
ELSE(UNIX OR OPENSSL_FOUND)
	set_target_properties(licensecc_static PROPERTIES WITH_OPENSSL 0)
	target_link_libraries(licensecc_static PUBLIC base ${EXTERNAL_LIBS})
ENDIF(UNIX OR OPENSSL_FOUND)

target_include_directories(licensecc_static
    INTERFACE 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

add_library(licensecc::licensecc_static ALIAS licensecc_static)
install(TARGETS licensecc_static EXPORT licensecc
	ARCHIVE DESTINATION ${main_lib_dest}/${LCC_PROJECT_NAME})
	
install(EXPORT licensecc NAMESPACE licensecc:: DESTINATION  ${main_lib_dest}/${LCC_PROJECT_NAME}/cmake)
export(EXPORT licensecc NAMESPACE licensecc:: FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.cmake)
