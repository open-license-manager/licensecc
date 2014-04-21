ADD_LIBRARY(os STATIC
	os-linux.c
    os-linux.cpp
)

target_link_libraries(
     os
     base
     ${EXTERNAL_LIBS}
)
