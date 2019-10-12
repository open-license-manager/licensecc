add_subdirectory("os")
add_subdirectory("base")
add_subdirectory("ini")
add_subdirectory("locate")


ADD_LIBRARY(licensecc_static STATIC
    license++.cpp
    LicenseReader.cpp
    pc-identifiers.c
)

target_link_libraries(
     licensecc_static
     ini
     locators
     os
)

install(TARGETS licensecc_static ARCHIVE DESTINATION lib)
install(FILES api/datatypes.h api/license++.h DESTINATION include/api)
install(FILES base/base.h DESTINATION include/base)
install(FILES pc-identifiers.h DESTINATION include/)
install(FILES ini/SimpleIni.h ini/ConvertUTF.h DESTINATION include/ini)
