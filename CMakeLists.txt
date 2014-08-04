cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

#version variables, major and minor must be 1 character only
SET(LICENSECC_MAJOR_VERSION 1)
SET(LICENSECC_MINOR_VERSION 0)
SET(LICENSECC_PATCH_VERSION 0 CACHE STRING "License++ patch version string")
SET(LICENSECC_INT_VERSION "${LICENSECC_MAJOR_VERSION}${LICENSECC_MINOR_VERSION}${LICENSECC_PATCH_VERSION}")
SET(LICENSECC_VERSION "${LICENSECC_MAJOR_VERSION}.${LICENSECC_MINOR_VERSION}.${LICENSECC_PATCH_VERSION}")
SET(LICENSECC_SHORT_LICENSE "BSD Software License")

# add this options before PROJECT keyword
SET(CMAKE_DISABLE_SOURCE_CHANGES OFF)
SET(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "CMake verbose" FORCE)

project (license++ C CXX)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/modules/")

if(WIN32)
	SET(PLATFORM_LIBS "")
	if(CMAKE_CL_64)
		SET(CMAKE_EXE_LINKER_FLAGS "/machine:X64 /INCREMENTAL:NO" CACHE STRING "Linker flags")
	endif(CMAKE_CL_64)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib ) #${CMAKE_FIND_LIBRARY_SUFFIXES}
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
else(WIN32)
    SET(PLATFORM_LIBS "m")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fmessage-length=0 -std=c++11 -Wall -Wuninitialized -fPIC") 
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fmessage-length=0 -Wall -Wuninitialized -fPIC") 
    SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -pipe" CACHE INTERNAL "Linker flags")
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -Wl,--strip-all")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a) #.so for dynamic libraries
    #you need to "force" the change in cmake_install_prefix after the project keyword
	IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		SET(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path prefix" FORCE)
	ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32)



if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(HAVE_64BIT_SIZE_T 1)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(HAVE_64BIT_SIZE_T 0)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)

#find_package(CryptoPP REQUIRED)
#include_directories(${CRYPTOPP_INCLUDE_DIRS})
#link_directories ( ${Boost_LIBRARY_DIRS} )


SET(Boost_USE_STATIC_LIBS ON)
# static runtime requires /MT 
#SET(Boost_USE_MULTITHREADED ON) #SET(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost COMPONENTS thread date_time program_options filesystem system regex unit_test_framework)
find_package(Boost COMPONENTS date_time program_options filesystem system unit_test_framework)
#include_directories(${Boost_INCLUDE_DIRS})
#link_directories ( ${Boost_LIBRARY_DIRS} )


if(NOT WIN32)	
	#find a static version of openssl crypto library
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	list(APPEND EXTERNAL_LIBS ${OPENSSL_CRYPTO_LIBRARY})
	FIND_PACKAGE(Dbus REQUIRED)
	include_directories(${DBUS_INCLUDE_DIR})
	include_directories(${DBUS_ARCH_INCLUDE_DIR})
	list(APPEND EXTERNAL_LIBS ${DBUS_LIBRARIES})
endif(NOT WIN32)

FIND_PACKAGE(Doxygen)

IF (DOXYGEN_FOUND)

	SET(DOXYGEN_INPUT ${CMAKE_BINARY_DIR}/doc/Doxyfile)
	SET(DOXYGEN_OUTPUT ${CMAKE_BINARY_DIR}/doc/doxy_output)
	
	configure_file (
	  "doc/Doxyfile.in"
	  "doc/Doxyfile"
	)
	
	ADD_CUSTOM_COMMAND(
	  OUTPUT ${DOXYGEN_OUTPUT}
	  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
	  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
	  COMMAND ${CMAKE_COMMAND} -E echo "Done."
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	  DEPENDS ${DOXYGEN_INPUT})
	
	ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})
	
	ADD_CUSTOM_TARGET(apidoc_forced
	  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
	  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
	  COMMAND ${CMAKE_COMMAND} -E echo "Done."
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF (DOXYGEN_FOUND)

configure_file (
	"src/build_properties.h.in" 
	"${CMAKE_BINARY_DIR}/build_properties.h"
)
#include build directory to find build_properties.h
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(src)

#test are done with boost_tests:disable them if boost not found.
IF(Boost_FOUND)
	ENABLE_TESTING()
	INCLUDE(CTest)
	IF(BUILD_TESTING)
	  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
	  MARK_AS_ADVANCED(BUILDNAME)
	ENDIF(BUILD_TESTING)
	add_subdirectory(test)
ENDIF(Boost_FOUND)

#include(build/modules/CmakeDebugVariables.txt)

message("")
message( STATUS "C compiler: " ${CMAKE_C_COMPILER})
message( STATUS "C compiler flags: " ${CMAKE_C_FLAGS})
if(CMAKE_BUILD_TYPE)
  message( STATUS "Build type: " ${CMAKE_BUILD_TYPE})
endif(CMAKE_BUILD_TYPE)
message( STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})
IF(Boost_FOUND)
	message( STATUS "Boost found: running a complete build.")
ELSE(Boost_FOUND)
	message( STATUS "Boost not found: tests, pc-identifier will be skipped")
ENDIF(Boost_FOUND)
if(DOXYGEN_FOUND)
  message(STATUS "Doxygen: " ${DOXYGEN_EXECUTABLE})
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen: not found.")
endif(DOXYGEN_FOUND)
