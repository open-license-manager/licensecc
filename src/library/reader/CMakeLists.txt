
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

ADD_LIBRARY(reader_dynl STATIC
    ConvertUTF.c
    LicenseReader.cpp
)

SET_TARGET_PROPERTIES(reader_dynl PROPERTIES LINK_SEARCH_START_STATIC OFF)
SET_TARGET_PROPERTIES(reader_dynl PROPERTIES LINK_SEARCH_END_STATIC OFF)
target_link_libraries(
     reader_dynl
     os
     #base
     #${EXTERNAL_LIBS}
)