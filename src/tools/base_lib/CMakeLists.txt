
if(WIN32)
ADD_LIBRARY(
    tools_base STATIC
    CryptoHelper.cpp
    win/CryptoHelperWindows.cpp
)
else(WIN32)
ADD_LIBRARY(
    tools_base STATIC
    CryptoHelper.cpp
    linux/CryptoHelperLinux.cpp
)
ENDIF(WIN32)

target_link_libraries(
     tools_base
     ${EXTERNAL_LIBS}
)
