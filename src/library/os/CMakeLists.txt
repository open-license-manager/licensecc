IF(UNIX OR OPENSSL_FOUND)
	IF(UNIX)
		add_library(os OBJECT
		    openssl/signature_verifier.cpp
		    linux/execution_environment.cpp
		    linux/cpu_info.cpp
		    linux/os-linux.c 
		    linux/network_id.c)
	ELSE(UNIX)
  	    add_library(os OBJECT openssl/signature_verifier.cpp windows/os-win.c)
	ENDIF(UNIX)
ELSE(UNIX OR OPENSSL_FOUND)
    target_sources(os OBJECT windows/signature_verifier.cpp windows/os-win.c)
ENDIF(UNIX OR OPENSSL_FOUND)

if(CODE_COVERAGE AND UNIX)
	  MESSAGE(STATUS "Enabling code coverage")
	  target_compile_options(os PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)