ADD_LIBRARY(os STATIC
    os-linux.cpp
)


target_link_libraries(
     os
     base
     ${EXTERNAL_LIBS}
)