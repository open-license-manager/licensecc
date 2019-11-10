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
	set(LCC_PROJECT_NAME "DEFAULT") 
ENDIF(NOT LCC_PROJECT_NAME)

#base folder where projects are stored (/projects)
IF(NOT LCC_PROJECTS_BASE_DIR)
	set(LCC_PROJECTS_BASE_DIR "${CMAKE_SOURCE_DIR}/projects") 
ENDIF(NOT LCC_PROJECTS_BASE_DIR)

project (licensecc C CXX )
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#find olm executable or build it.
find_package(olm REQUIRED) 

#load the current project from files or find it from environment variables or create a default one
set(LCC_INCLUDE_DIR "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/include/${PROJECT_NAME}/${LCC_PROJECT_NAME}" )
set(LCC_PROJECT_PUBLIC_KEY "${LCC_INCLUDE_DIR}/public_key.h" )
set(LCC_PROJECT_PRIVATE_KEY "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/private_key.rsa" )

add_custom_command(
  OUTPUT "${LCC_PROJECT_PUBLIC_KEY}" "${LCC_PROJECT_PRIVATE_KEY}"
  COMMAND license_generator::olm project initialize -s "${PROJECT_SOURCE_DIR}/src" -n "${LCC_PROJECT_NAME}" -p "${LCC_PROJECTS_BASE_DIR}"
  COMMENT "generating ${LCC_PROJECT_PUBLIC_KEY}"
  USES_TERMINAL
)

add_custom_target(project_initialize DEPENDS "${LCC_PROJECT_PUBLIC_KEY}" "${LCC_PROJECT_PRIVATE_KEY}")

configure_file (
	"src/templates/build_properties.h.in" 
	"${LCC_INCLUDE_DIR}/build_properties.h"
)
include_directories( ${LCC_INCLUDE_DIR} )

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

add_subdirectory(src)

SET( Boost_USE_STATIC_LIBS ON )
find_package(Boost REQUIRED COMPONENTS date_time unit_test_framework program_options system filesystem)

#set below in case of dynamic linking in debug.
#set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BOOST_TEST_DYN_LINK>)

#if boost is found enable tests
IF(Boost_FOUND)
	INCLUDE(CTest)
	IF(BUILD_TESTING)
		SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
		MARK_AS_ADVANCED(BUILDNAME)
		add_definitions(-DBOOST_ALL_NO_LIB) #Disable Boost Microsoft magic, all dependencies are handled by cmake
		add_definitions(-DBOOST_LIB_DIAGNOSTIC) #Check it is really disabled
		include_directories(${Boost_INCLUDE_DIRS}) 
#		add_subdirectory(test)
	ENDIF(BUILD_TESTING)
ELSE(Boost_FOUND)
	message(WARNING "Boost not found, disabling tests")
ENDIF(Boost_FOUND)

