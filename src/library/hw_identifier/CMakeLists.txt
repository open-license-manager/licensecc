add_library(hw_identifier OBJECT
	hw_identifier_facade.cpp
	default_strategy.cpp
	ethernet.cpp
	disk_strategy.cpp
	identification_strategy.cpp
	hw_identifier.cpp
)

# Ensure project headers are generated before compiling
add_dependencies(hw_identifier project_initialize)

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(hw_identifier PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)