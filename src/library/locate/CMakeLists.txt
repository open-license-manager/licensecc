add_library(locate OBJECT
    ApplicationFolder.cpp
    EnvironmentVarLocation.cpp
    EnvironmentVarData.cpp
    ExternalDefinition.cpp
    LocatorStrategy.cpp
    LocatorFactory.cpp
)

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(locate PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)