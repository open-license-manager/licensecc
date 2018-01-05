IF(WIN32)
    add_subdirectory("os/win")
ELSE(WIN32)
    add_subdirectory("os/linux")    
ENDIF(WIN32)
add_subdirectory("base") 
add_subdirectory("ini")   

ADD_LIBRARY(license++_static STATIC
    license++.cpp
    LicenseReader.cpp
    pc-identifiers.c
)

target_link_libraries(
     license++_static
     ini
     os
)

install(TARGETS license++_static ARCHIVE DESTINATION lib)
install(FILES api/datatypes.h api/license++.h DESTINATION include/api)
install(FILES base/base.h DESTINATION include/base)
install(FILES pc-identifiers.h DESTINATION include/)
