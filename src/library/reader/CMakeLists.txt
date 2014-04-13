
ADD_LIBRARY(reader STATIC
    ConvertUTF.c
    LicenseReader.cpp
)

target_link_libraries(
     reader
     os
     base
     ${EXTERNAL_LIBS}
)

