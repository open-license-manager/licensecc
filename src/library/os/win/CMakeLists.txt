ADD_LIBRARY(os STATIC
	os-win.c
    os-win.cpp
)

target_link_libraries(
     os
     base
     ${EXTERNAL_LIBS}
)
