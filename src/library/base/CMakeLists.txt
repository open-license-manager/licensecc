ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
    FileUtils.cpp
    logger.c
    base64.c
)

if(CODE_COVERAGE AND UNIX)
	  MESSAGE(STATUS "Enabling code coverage")
	  target_compile_options(base PUBLIC
	    -O0        # no optimization
	    -g         # generate debug info
	    --coverage # sets all required flags
	  )
	  target_link_libraries(base PUBLIC gcov)
endif(CODE_COVERAGE AND UNIX)

add_dependencies( base project_initialize )

install(TARGETS base EXPORT licensecc ARCHIVE DESTINATION lib/${PROJECT_NAME}/${LCC_PROJECT_NAME})

