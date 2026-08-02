add_subdirectory("base")
add_subdirectory("os")
add_subdirectory("locate")
add_subdirectory("hw_identifier")

ADD_LIBRARY(licensecc_static STATIC
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

if(CODE_COVERAGE AND UNIX)
	target_link_options(licensecc_static PUBLIC --coverage)
	list(APPEND EXTERNAL_LIBS gcov)
endif(CODE_COVERAGE AND UNIX)

define_property(TARGET PROPERTY WITH_OPENSSL BRIEF_DOCS "need openssl to compile" FULL_DOCS "ff")

IF(OPENSSL_FOUND)
	target_compile_definitions(licensecc_static PUBLIC HAS_OPENSSL)
	set_target_properties(licensecc_static PROPERTIES WITH_OPENSSL 1)
	target_link_libraries(licensecc_static PUBLIC OpenSSL::Crypto ${EXTERNAL_LIBS} ${CMAKE_DL_LIBS})
    #check if really required. it is required for the generator
    if(UNIX)
        set_target_properties(licensecc_static PROPERTIES POSITION_INDEPENDENT_CODE ${STATIC_RUNTIME})
    endif(UNIX) 
ELSE(UNIX OR OPENSSL_FOUND)
	set_target_properties(licensecc_static PROPERTIES WITH_OPENSSL 0)
	target_link_libraries(licensecc_static PUBLIC ${EXTERNAL_LIBS})
ENDIF(OPENSSL_FOUND)

target_include_directories(licensecc_static
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}/${LCC_PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<BUILD_INTERFACE:${LCC_INCLUDE_DIR}>
)

add_library(licensecc::licensecc_static ALIAS licensecc_static)
install(TARGETS licensecc_static EXPORT licensecc
	ARCHIVE DESTINATION ${main_lib_dest}/${LCC_PROJECT_NAME})
	
install(EXPORT licensecc NAMESPACE licensecc:: DESTINATION  ${main_lib_dest}/${LCC_PROJECT_NAME}/cmake)
export(EXPORT licensecc NAMESPACE licensecc:: FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.cmake)
