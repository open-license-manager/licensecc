#TODO guess address_model 64 in boost
#make it compatible with find_boost (exchange lowercase to camelcase)
#fix shared libraries activation

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

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

include(utilities)
string(TOLOWER ${CMAKE_BUILD_TYPE} _CMAKE_BUILD_TYPE)
EVAL_CONDITION(RELEASE_BUILD ${_CMAKE_BUILD_TYPE} STREQUAL "release")

if(WIN32)
	SET(PLATFORM_LIBS "")
	if(CMAKE_CL_64)
		SET(CMAKE_EXE_LINKER_FLAGS "/machine:X64 /INCREMENTAL:NO" CACHE STRING "Linker flags")
	endif(CMAKE_CL_64)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib ) #${CMAKE_FIND_LIBRARY_SUFFIXES}
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
else(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fmessage-length=0 -std=c++11 -Wuninitialized -fPIC") #-Wall
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fmessage-length=0 -Wall -Wuninitialized -fPIC") 
    #use static libraries in release to make it easier to distribute it
    #SET(Boost_USE_STATIC_RUNTIME ${RELEASE_BUILD})
    SET(Boost_USE_STATIC_LIBS ${RELEASE_BUILD})
    if(${RELEASE_BUILD})
        SET(CMAKE_FIND_LIBRARY_SUFFIXES .a) #.so for dynamic libraries
    else()
        SET(CMAKE_FIND_LIBRARY_SUFFIXES .a .so) 
    endif()
    set (CMAKE_EXE_LINKER_FLAGS  "-pthread")
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--strip-all -static-libstdc++") #-static 
    #you need to "force" the change in cmake_install_prefix after the project keyword
	IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		SET(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path prefix" FORCE)
	ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32)
#TODO uncomment: source temporarily broken
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BOOST_TEST_DYN_LINK>)


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(HAVE_64BIT_SIZE_T 1)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(HAVE_64BIT_SIZE_T 0)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

#TODO: determine target arch.
SET( TargetArchitecture "X86_64")

include(add_boost)
# static runtime requires /MT 
#SET(Boost_USE_MULTITHREADED ON) #SET(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost COMPONENTS thread date_time program_options filesystem system regex unit_test_framework)
#find_package(Boost COMPONENTS date_time program_options filesystem system unit_test_framework)


if(NOT WIN32)	
	#find a static version of openssl crypto library
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	list(APPEND EXTERNAL_LIBS ${OPENSSL_CRYPTO_LIBRARY})
	FIND_PACKAGE(Dbus REQUIRED)
	include_directories(${DBUS_INCLUDE_DIR})
	include_directories(${DBUS_ARCH_INCLUDE_DIR})
	list(APPEND EXTERNAL_LIBS ${DBUS_LIBRARIES})
	list(APPEND EXTERNAL_LIBS dl)
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

INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)
add_subdirectory(test)

#include(build/modules/CmakeDebugVariables.txt)

message("")
message( STATUS "C compiler: " ${CMAKE_C_COMPILER})
message( STATUS "C compiler flags: " ${CMAKE_C_FLAGS})
if(CMAKE_BUILD_TYPE)
  message( STATUS "Build type: " ${CMAKE_BUILD_TYPE})
endif(CMAKE_BUILD_TYPE)
message( STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})
if(DOXYGEN_FOUND)
  message(STATUS "Doxygen: " ${DOXYGEN_EXECUTABLE})
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen: not found.")
endif(DOXYGEN_FOUND)
