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

install(TARGETS licensecc_static ARCHIVE DESTINATION lib)
install(FILES api/datatypes.h api/license++.h DESTINATION include/api)
install(FILES base/base.h DESTINATION include/base)
install(FILES pc-identifiers.h DESTINATION include/)
install(FILES ini/SimpleIni.h ini/ConvertUTF.h DESTINATION include/ini)
