cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

#version variables, major and minor must be 1 character only
SET(LICENSECC_MAJOR_VERSION 1)
SET(LICENSECC_MINOR_VERSION 1)
SET(LICENSECC_PATCH_VERSION 0 CACHE STRING "Licensecc patch version string")
SET(LICENSECC_INT_VERSION "${LICENSECC_MAJOR_VERSION}${LICENSECC_MINOR_VERSION}${LICENSECC_PATCH_VERSION}")
SET(LICENSECC_VERSION "${LICENSECC_MAJOR_VERSION}.${LICENSECC_MINOR_VERSION}.${LICENSECC_PATCH_VERSION}")
SET(LICENSECC_SHORT_LICENSE "BSD Software License")

SET(CMAKE_DISABLE_SOURCE_CHANGES OFF) #keys are generated in the source tree by default
SET(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

IF(NOT LCC_PROJECT_NAME)
	message(WARNING "You should define a variable LCC_PROJECT_NAME containing the name of the software you want to add a license to."
		"A mock product named DEFAULT has been added for you.")
	set(LCC_PROJECT_NAME "DEFAULT" CACHE STRING "Project name (name of the software for which you want to issue a license)") 
ENDIF(NOT LCC_PROJECT_NAME)

#base folder where projects are stored (/projects)
IF(NOT LCC_PROJECTS_BASE_DIR)
	set(LCC_PROJECTS_BASE_DIR "${CMAKE_SOURCE_DIR}/projects") 
ENDIF(NOT LCC_PROJECTS_BASE_DIR)

project (licensecc C CXX )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#find lcc executable or build it.
find_package(lcc REQUIRED) 

#In case the build system doesn't support binfmt misc
IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" ) AND CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_CROSSCOMPILING_EMULATOR )
	SET(CMAKE_CROSSCOMPILING_EMULATOR "wine")	
ENDIF()

if(MSVC)
	include(target_arch)
	target_architecture( TARGET_ARCHITECTURE )
	message(STATUS "architecture detected: ${TARGET_ARCHITECTURE}")
	
	#cmake > 3.15 handle the /MD flag more nicely than this
	if(${STATIC_RUNTIME})
		string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
		string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
		string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
		string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
		SET(Boost_USE_STATIC_RUNTIME ON)
	endif(${STATIC_RUNTIME})

	add_definitions("/D _CRT_SECURE_NO_WARNINGS")
else(MSVC)
    if(MINGW)
        list(APPEND EXTERNAL_LIBS "-lcrypt32 -lws2_32 -liphlpapi")
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    endif(MINGW)
endif(MSVC)

if(UNIX) #this is true for all the linux systems but not for cross compiling "linux->windows"
	#find a static version of openssl crypto library
	SET ( OPENSSL_USE_STATIC_LIBS ON )
	find_package(OpenSSL REQUIRED COMPONENTS Crypto)
	include_directories(${OPENSSL_INCLUDE_DIR})
    #list(APPEND EXTERNAL_LIBS ${OPENSSL_CRYPTO_LIBRARY})
	MESSAGE(STATUS "Found openssl version ${OPENSSL_VERSION} ")
    find_package(Threads)
	if(THREADS_HAVE_PTHREAD_ARG)
  		list(APPEND EXTERNAL_LIBS "-pthread")
	else(THREADS_HAVE_PTHREAD_ARG)
		if(CMAKE_THREAD_LIBS_INIT)
  			 list(APPEND EXTERNAL_LIBS "${CMAKE_THREAD_LIBS_INIT}")
		endif()
	endif(THREADS_HAVE_PTHREAD_ARG)
	#Zlib required when openssl version < 1.0.1f
	#SET ( ZLIB_USE_STATIC_LIBS ON )
	#find_package(ZLIB REQUIRED)
	#MESSAGE(STATUS "Found zlib version ${ZLIB_VERSION} ")
	find_program( MEMORYCHECK_COMMAND valgrind )
	set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
	if(USE_DBUS_IDENTIFIER)
	    FIND_PACKAGE(Dbus REQUIRED)
	    add_definitions(-DUSE_DBUS)
	    include_directories(${DBUS_INCLUDE_DIR})
	    include_directories(${DBUS_ARCH_INCLUDE_DIR})
	    list(APPEND EXTERNAL_LIBS ${DBUS_LIBRARIES})
	endif(USE_DBUS_IDENTIFIER)    
	#if(NOT MINGW)
 	#	list(APPEND EXTERNAL_LIBS ${CMAKE_DL_LIBS}) 
	#endif(NOT MINGW)
endif(UNIX)

#initialize project
#load the current project from files or find it from environment variables or create a default one
set(LCC_INCLUDE_DIR "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/include/${PROJECT_NAME}/${LCC_PROJECT_NAME}" )
set(LCC_PROJECT_PUBLIC_KEY "${LCC_INCLUDE_DIR}/public_key.h" )
set(LCC_PROJECT_PRIVATE_KEY "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/private_key.rsa" )

add_custom_command(
  OUTPUT "${LCC_PROJECT_PUBLIC_KEY}" "${LCC_PROJECT_PRIVATE_KEY}"
  COMMAND license_generator::lcc project initialize -t "${PROJECT_SOURCE_DIR}/src/templates" -n "${LCC_PROJECT_NAME}" -p "${LCC_PROJECTS_BASE_DIR}"
  COMMENT "generating ${LCC_PROJECT_PUBLIC_KEY}"
  USES_TERMINAL
)
add_custom_target(project_initialize DEPENDS "${LCC_PROJECT_PUBLIC_KEY}" "${LCC_PROJECT_PRIVATE_KEY}")

include_directories( ${LCC_INCLUDE_DIR} ${CMAKE_BINARY_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")

message( STATUS "C compiler        : " ${CMAKE_C_COMPILER})
message( STATUS "C compiler flags  : " ${CMAKE_C_FLAGS})
message( STATUS "CXX compiler      : " ${CMAKE_CXX_COMPILER})
message( STATUS "CXX compiler flags: " ${CMAKE_CXX_FLAGS})
message( STATUS "CXX compiler flags debug : " ${CMAKE_CXX_FLAGS_DEBUG})
message( STATUS "CXX compiler flags release: " ${CMAKE_CXX_FLAGS_RELEASE})
if(CMAKE_BUILD_TYPE)
  message( STATUS "Build type        : " ${CMAKE_BUILD_TYPE})
endif(CMAKE_BUILD_TYPE)
message( STATUS "Install prefix    : " ${CMAKE_INSTALL_PREFIX})
message( STATUS "Project name      : " ${LCC_PROJECT_NAME} )
message( STATUS "Project base dir  : " ${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME} )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
add_subdirectory(src)

#boost is required only for tests
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
SET( Boost_USE_STATIC_LIBS ON )
find_package(Boost COMPONENTS unit_test_framework system filesystem)

#if boost is found enable tests
IF(Boost_FOUND)
	INCLUDE(CTest)
	IF(BUILD_TESTING)
		SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
		MARK_AS_ADVANCED(BUILDNAME)
		add_subdirectory(test)
	ENDIF(BUILD_TESTING)
ELSE(Boost_FOUND)
	message(WARNING "Boost not found, disabling tests")
ENDIF(Boost_FOUND)
