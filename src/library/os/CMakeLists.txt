IF(WIN32)
    ADD_LIBRARY(os STATIC
        os.c
        os-win.c)
ELSE(WIN32)
    ADD_LIBRARY(os STATIC
        os.c
        os-linux.c
        network_id.c)
ENDIF(WIN32)

target_link_libraries(
     os
     base
     ${EXTERNAL_LIBS}
)

install(TARGETS os ARCHIVE DESTINATION lib)
