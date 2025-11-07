IF(UNIX OR OPENSSL_FOUND)
	IF(APPLE)
		# macOS-specific files
		add_library(os OBJECT
		    openssl/signature_verifier.cpp
            execution_environment_common.cpp
		    macos/execution_environment.cpp
		    macos/cpu_info.cpp
		    macos/dmi_info.cpp
		    macos/network.cpp
		    macos/os_macos.cpp)
	ELSEIF(UNIX)
		# Linux-specific files
		add_library(os OBJECT
		    openssl/signature_verifier.cpp
            execution_environment_common.cpp
		    linux/execution_environment.cpp
		    linux/cpu_info.cpp
		    linux/dmi_info.cpp
		    linux/network.cpp
		    linux/os_linux.cpp)
	ELSE()
	    #windows and openssl
  	    add_library(os OBJECT
  	    windows/cpu_info.cpp
  	    windows/dmi_info.cpp
  	    openssl/signature_verifier.cpp
        execution_environment_common.cpp
        windows/execution_environment.cpp
  	    windows/os_win.cpp
  	    windows/network.cpp)
	ENDIF()
	target_include_directories(os PUBLIC ${OPENSSL_INCLUDE_DIR})
ELSE(UNIX OR OPENSSL_FOUND)
#windows no openssl
    add_library(os OBJECT
        windows/cpu_info.cpp
        windows/dmi_info.cpp 
        windows/signature_verifier.cpp 
        execution_environment_common.cpp 
        windows/execution_environment.cpp
        windows/os_win.cpp
        windows/network.cpp)
ENDIF(UNIX OR OPENSSL_FOUND)

#dependency of signature_verifier.cpp
add_dependencies( os project_initialize )

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(os PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)