ADD_LIBRARY(base OBJECT
    EventRegistry.cpp
    StringUtils.cpp
    FileUtils.cpp
    base64.cpp
    logger.c
)

add_dependencies( base project_initialize )

if(CODE_COVERAGE AND UNIX)
	  MESSAGE(STATUS "Enabling code coverage")
	  target_compile_options(base PUBLIC
	    -O0        # no optimization
	    -g         # generate debug info
	    --coverage # sets all required flags
	  )
endif(CODE_COVERAGE AND UNIX)


