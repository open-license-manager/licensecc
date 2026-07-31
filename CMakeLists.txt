cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

## configuration variables
## USE_OPENSSL: Enable/Disable OpenSSL support. If disabled, the generator will use Windows Crypto API on Windows and will fail to compile on Linux.
## STATIC_RUNTIME: Enable/Disable linking to the static version of the c libraries (/MT /MD in windows)
## Boost_ROOT: Hint for a path to the Boost installation. If not set, CMake will try to find Boost in the system paths.
## OpenSSL_ROOT: Hint for a path to the Openssl installation. 
## LCC_PROJECT_NAME
## LCC_PROJECTS_BASE_DIR

SET(CMAKE_DISABLE_SOURCE_CHANGES OFF) #keys are generated in the source tree by default
SET(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

IF(NOT LCC_PROJECT_NAME)
	message(STATUS "You should define a variable LCC_PROJECT_NAME containing the name of the software you want to add a license to."
		"A mock project named DEFAULT has been added for you.")
	set(LCC_PROJECT_NAME "DEFAULT" CACHE STRING "Project name (name of the software for which you want to issue a license)") 
ENDIF(NOT LCC_PROJECT_NAME)

#base folder where projects are stored (/projects)
IF(NOT LCC_PROJECTS_BASE_DIR)
	set(LCC_PROJECTS_BASE_DIR "${CMAKE_SOURCE_DIR}/projects") 
ENDIF(NOT LCC_PROJECTS_BASE_DIR)

IF(POLICY CMP0167)
	cmake_policy(SET CMP0167 OLD)
ENDIF(POLICY CMP0167)
IF(POLICY CMP0144)
	cmake_policy(SET CMP0144 NEW)
ENDIF(POLICY CMP0144)
cmake_policy(SET CMP0048 NEW)

project (licensecc VERSION 2.1.0 LANGUAGES CXX DESCRIPTION "Copy protection and licensing library")
SET(licensecc_SHORT_LICENSE "BSD Software License")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

#find lccgen executable or build it.
find_package(lccgen REQUIRED) 

#Not sure about this. My system doesn't support binfmt misc
IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" ) AND CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_CROSSCOMPILING_EMULATOR )
	SET(CMAKE_CROSSCOMPILING_EMULATOR "wine")	
ENDIF()

option(STATIC_RUNTIME "Compile a static executable or a dynamic linked one" ON)
message( STATUS "Static   runtime          : ${STATIC_RUNTIME}")

option(OPENSSL_USE_STATIC_LIBS "Use static libraries for openssl" ON)
option(Boost_USE_STATIC_LIBS  "Use static libraries for Boost" ON)

if(UNIX) #this is true for all the linux systems but not for cross compiling "linux->windows"
	IF(${STATIC_RUNTIME})
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
		unset(CMAKE_DL_LIBS)
		add_definitions(-DBOOST_TEST_STATIC_LINK)
	ENDIF(${STATIC_RUNTIME})
	find_package(Threads)
	if(THREADS_HAVE_PTHREAD_ARG)
  		list(APPEND EXTERNAL_LIBS "-pthread")
	else(THREADS_HAVE_PTHREAD_ARG)
		if(CMAKE_THREAD_LIBS_INIT)
  			 list(APPEND EXTERNAL_LIBS "${CMAKE_THREAD_LIBS_INIT}")
		endif()
	endif(THREADS_HAVE_PTHREAD_ARG)
	
	#openssl always required in linux
	IF(DEFINED USE_OPENSSL AND NOT USE_OPENSSL) #the user has explicitly disabled openssl, but we are in linux we need to enable it.
		message(FATAL_ERROR "USE_OPENSSL is set to OFF, but we are in linux. Please enable it.")
	ENDIF(DEFINED USE_OPENSSL AND NOT USE_OPENSSL)
	find_package(OpenSSL REQUIRED COMPONENTS Crypto QUIET)
	add_definitions(-DHAS_OPENSSL)
	message( STATUS "OpenSSL libraries         :  ${OPENSSL_CRYPTO_LIBRARIES}")

	set(CTEST_MEMORYCHECK_TYPE "Valgrind")
	find_program( MEMORYCHECK_COMMAND valgrind )
	IF( MEMORYCHECK_COMMAND STREQUAL "MEMORYCHECK_COMMAND-NOTFOUND" )
		message( STATUS "valgrind                  : Not Found")
	ELSE()
		message( STATUS "valgrind                  : ${MEMORYCHECK_COMMAND}" )
	ENDIF()
	set( MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 --leak-check=full" )
	#set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
	set(main_lib_dest "lib/${PROJECT_NAME}")
	#https://cmake.org/cmake/help/latest/command/find_package.html
	set(pkg_config_dest "lib/cmake/${PROJECT_NAME}")
else(UNIX) #windows and linux crosscompiling for windows target
	
	SET (OPENSSL_MSVC_STATIC_RT ${STATIC_RUNTIME})
	IF(DEFINED USE_OPENSSL)
		if(USE_OPENSSL)
			MESSAGE(VERBOSE "Openssl (licensecc)       : forced by the user")
			find_package(OpenSSL REQUIRED COMPONENTS Crypto QUIET)
		else(USE_OPENSSL) #defined but evaluate
			MESSAGE(VERBOSE "Openssl (licensecc)       : disabled by user.")
			set(OpenSSL_FOUND FALSE)
		endif(USE_OPENSSL)
	else(DEFINED USE_OPENSSL)
		SET (OPENSSL_MSVC_STATIC_RT TRUE)
		find_package(OpenSSL COMPONENTS Crypto QUIET)
	endif(DEFINED USE_OPENSSL)
	IF(OpenSSL_FOUND)
		add_definitions(-DHAS_OPENSSL)
		include_directories(${OPENSSL_INCLUDE_DIR})
		MESSAGE(STATUS "Openssl (licensecc)       : ${OPENSSL_VERSION}")
		MESSAGE(STATUS "Openssl libraries         : ${OPENSSL_CRYPTO_LIBRARY}")
	ELSE(OpenSSL_FOUND) 
		MESSAGE(STATUS "Openssl (licensecc)       : not found or not enabled - using windows cryptography.")
		list(APPEND EXTERNAL_LIBS "bcrypt" "crypt32" "ws2_32" "iphlpapi")
	ENDIF(OpenSSL_FOUND)

	if(MSVC)
		include(target_arch)
		target_architecture( TARGET_ARCHITECTURE )
		message(STATUS "Architecture detected     : ${TARGET_ARCHITECTURE}")
		if(${STATIC_RUNTIME})
			SET(Boost_USE_STATIC_RUNTIME ON)
			unset(CMAKE_DL_LIBS)
			set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
		ELSE(${STATIC_RUNTIME})
			set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE INTERNAL "")
		endif(${STATIC_RUNTIME})
		add_definitions("/D _CRT_SECURE_NO_WARNINGS")
		add_compile_options("/EHsc")
	else(MSVC)
	    if(MINGW)
		list(APPEND EXTERNAL_LIBS "-lbcrypt" "-lcrypt32" "-lws2_32" "-liphlpapi")
	        SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libstdc++")
	    endif(MINGW)
	endif(MSVC)
	set(main_lib_dest "${PROJECT_NAME}")  
	#https://cmake.org/cmake/help/latest/command/find_package.html
	set(pkg_config_dest "cmake/${PROJECT_NAME}")
endif(UNIX)

#initialize project
#load the current project from files or find it from environment variables or create a default one
set(LCC_INCLUDE_DIR "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/include/${PROJECT_NAME}/${LCC_PROJECT_NAME}" )
set(LCC_PROJECT_PUBLIC_KEY "${LCC_INCLUDE_DIR}/public_key.h" )
set(LCC_PROJECT_PRIVATE_KEY "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/private_key.rsa" )

add_custom_target(project_initialize
  COMMAND license_generator::lccgen project initialize -t "${PROJECT_SOURCE_DIR}/src/templates" -n "${LCC_PROJECT_NAME}" -p "${LCC_PROJECTS_BASE_DIR}"
  COMMENT "generating ${LCC_PROJECT_PUBLIC_KEY} and ${LCC_PROJECT_PRIVATE_KEY} if they don't already exist"
  USES_TERMINAL
)

include_directories( ${LCC_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include" )

message( STATUS "CXX compiler              : " ${CMAKE_CXX_COMPILER})
message( STATUS "CXX compiler flags        : " ${CMAKE_CXX_FLAGS})
message( STATUS "CXX compiler flags debug  : " ${CMAKE_CXX_FLAGS_DEBUG})
message( STATUS "CXX compiler flags release: " ${CMAKE_CXX_FLAGS_RELEASE})
if(CMAKE_BUILD_TYPE)
  message( STATUS "Build type                : " ${CMAKE_BUILD_TYPE})
endif(CMAKE_BUILD_TYPE)
message( STATUS "Install prefix            : " ${CMAKE_INSTALL_PREFIX})
message( STATUS "External libs             : " ${EXTERNAL_LIBS})
message( STATUS "Project name              : " ${LCC_PROJECT_NAME} )
message( STATUS "Project base dir          : " ${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME} )

add_subdirectory(src)

#bug in cmake win32 - can't find boost compiled with mingw 
if(WIN32 AND "x${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "x")
    message(WARNING "WIN32 compiler does not specify CMAKE_CXX_COMPILER_ARCHITECTURE_ID -- filling in manually")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")
    else()
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x86")
    endif()
    message(STATUS "Compiler architecture: ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endif()

#boost is required only for tests and inspector
#SET (Boost_USE_STATIC_RUNTIME ${STATIC_RUNTIME}) doesn't work well in Linux... 
SET( Boost_USE_STATIC_LIBS ON )
find_package(Boost COMPONENTS date_time filesystem program_options unit_test_framework OPTIONAL_COMPONENTS system)

#if boost is found enable tests and inspector
IF(Boost_FOUND)
	message( STATUS "Boost libs                : " ${Boost_LIBRARIES})
	message( STATUS "Boost dirs                : " ${Boost_LIBRARY_DIRS})	#set(MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1") #already set
	#set below in case of dynamic linking in debug (cmake 3.15?).
	#set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BOOST_TEST_DYN_LINK>)
	add_definitions(-DBOOST_ALL_NO_LIB) #Disable Boost Microsoft magic, all dependencies are handled by cmake
	add_definitions(-DBOOST_LIB_DIAGNOSTIC) #Check it is really disabled
	include_directories(${Boost_INCLUDE_DIRS}) 
	
	INCLUDE(CTest)
	IF(BUILD_TESTING)
		SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
		MARK_AS_ADVANCED(BUILDNAME)
		add_subdirectory(test)
	ENDIF(BUILD_TESTING)
	add_subdirectory("src/inspector")
ELSE(Boost_FOUND)
	message(WARNING "Boost not found, disabling tests and inspector")
ENDIF(Boost_FOUND)

#################################
# Documentation
#################################
find_package(Doxygen COMPONENTS dot)
find_package(Sphinx)
 
IF(Doxygen_FOUND AND Sphinx_FOUND)
    message(STATUS "Doxygen found, generating docs.")
    SET(DOXYGEN_EXCLUDE_PATTERNS "*/library/ini/*;*/doc/resources/*")
    SET(DOXYGEN_DISABLE_INDEX YES)
    #SET(DOXYGEN_GENERATE_TREEVIEW YES)
    SET(DOXYGEN_GENERATE_XML YES)
    SET(DOXYGEN_LAYOUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxygenLayout.xml")
	doxygen_add_docs(docs doc src include/licensecc ${LCC_INCLUDE_DIR} COMMENT "doxygen docs")
	#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/doc/resources/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/html")
	
	#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/doc)
	set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
 
	add_custom_target(documentation
                  COMMAND
                  ${SPHINX_EXECUTABLE} -b html
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  -Dbreathe_projects.licensecc=${CMAKE_CURRENT_BINARY_DIR}/xml
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")
    add_dependencies(documentation docs)
    
ELSE(Doxygen_FOUND AND Sphinx_FOUND)
	message(STATUS "Doxygen or Sphynx not found, not generating docs.")
ENDIF(Doxygen_FOUND AND Sphinx_FOUND)
install(DIRECTORY ${LCC_INCLUDE_DIR} DESTINATION include/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(FILES src/cmake/licensecc-config.cmake DESTINATION ${pkg_config_dest})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_BINARY_DIR}/licensecc-config-version.cmake COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_BINARY_DIR}/licensecc-config-version.cmake DESTINATION ${pkg_config_dest})


