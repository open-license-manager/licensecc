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
)

