IF(WIN32)
    add_subdirectory("os/win")
ELSE(WIN32)
    add_subdirectory("os/linux")    
ENDIF(WIN32)
add_subdirectory("base") 
add_subdirectory("reader")   

ADD_LIBRARY(license++_static STATIC
    license++.cpp
)

target_link_libraries(
     license++_static
     reader
     os
     ${EXTERNAL_LIBS}
)

ADD_LIBRARY(license++ SHARED
    license++.cpp
)

SET_TARGET_PROPERTIES(license++ PROPERTIES LINK_SEARCH_START_STATIC OFF)
SET_TARGET_PROPERTIES(license++ PROPERTIES LINK_SEARCH_END_STATIC OFF)

target_link_libraries(
     license++
     reader_dynl
     #os
     #${EXTERNAL_LIBS}
)
set_target_properties(license++ PROPERTIES PREFIX "lib")