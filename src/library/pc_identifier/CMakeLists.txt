add_library(pc_identifier OBJECT
	pc_identifier_facade.cpp
	identification_strategy.cpp
	ethernet.cpp
	disk_strategy.cpp
	pc_identifier.cpp
	default_strategy.cpp
	
	#pc-identifiers.c
)

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(pc_identifier PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)