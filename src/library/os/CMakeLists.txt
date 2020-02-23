IF(UNIX OR OPENSSL_FOUND)
	IF(UNIX)
		add_library(os OBJECT
		    openssl/signature_verifier.cpp
		    linux/execution_environment.cpp
		    cpu_info_common.cpp
		    linux/cpu_info.cpp
		    linux/network.cpp
		    linux/os-linux.c) 
	ELSE(UNIX)
  	    add_library(os OBJECT 
  	    cpu_info_common.cpp windows/cpu_info.cpp 
  	    openssl/signature_verifier.cpp 
  	    windows/execution_environment.cpp
  	    windows/isvm/Native.cpp
        windows/isvm/BIOSReader.cpp
  	    windows/os-win.c 
  	    windows/network.cpp)
	ENDIF(UNIX)
ELSE(UNIX OR OPENSSL_FOUND)
#windows no openssl
    add_library(os OBJECT
        cpu_info_common.cpp 
        windows/cpu_info.cpp 
        windows/signature_verifier.cpp 
        windows/execution_environment.cpp
        windows/isvm/Native.cpp
        windows/isvm/BIOSReader.cpp
        windows/os-win.c 
        windows/network.cpp)
ENDIF(UNIX OR OPENSSL_FOUND)

if(CODE_COVERAGE AND UNIX)
	  MESSAGE(STATUS "Enabling code coverage")
	  target_compile_options(os PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)