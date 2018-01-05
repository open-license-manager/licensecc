ADD_LIBRARY(os STATIC
	os-win.c
)

target_link_libraries(
     os
     base
     ${EXTERNAL_LIBS}
)

install(TARGETS os ARCHIVE DESTINATION lib)
