# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
Findolm
-------

Find or build the olm executable.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``license_generator::olm``
  The olm executable

If olm is not found this module will try to download it as a submodule
Git must be installed.

Input variables
^^^^^^^^^^^^^^^^

``OLM_LOCATION`` Hint for locating the olm executable

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``OLM_FOUND``
  True if the system has the Foo library.
``olm_VERSION``

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``OLM_EXECUTABLE``
  Path to the olm executable.

#]=======================================================================]

set(olm_names olm olm.exe)
set (failure_messge "Error finding olm executable.")
find_package(PkgConfig)

if(OLM_LOCATION)
	# First search the PATH and specific locations.
	find_program(OLM_EXECUTABLE
	  NAMES ${olm_names} HINTS ${OLM_LOCATION} DOC "olm command line client")
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(olm FOUND_VAR OLM_FOUND
                                       REQUIRED_VARS OLM_EXECUTABLE 
                                       FAIL_MESSAGE "Error finding olm executable. variable OLM_LOCATION non set correctly.")
    add_executable(license_generator::olm IMPORTED GLOBAL)                                            
	set_property(TARGET license_generator::olm PROPERTY IMPORTED_LOCATION ${OLM_EXECUTABLE})  
ELSE(OLM_LOCATION)
	find_package(olm REQUIRED NO_CMAKE_PATH) #try to find it without looping on this module
	message(STATUS ${olm_FOUND})
	IF(NOT olm_FOUND) 	
		find_package(Git QUIET)
		if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
		# Update submodules as needed
		    option(GIT_SUBMODULE "Check submodules during build" ON)
		    if(GIT_SUBMODULE)
		        message(STATUS "Submodule update")
#		        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#		                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#		                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
		        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
		            set(failure_messge  "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		        endif()
		    endif()
		endif()
		if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/license-generator/CMakeLists.txt")
		    set(failure_messge  "All the options to find olm executable failed. And i can't compile one from source GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
		endif()
		add_subdirectory("${PROJECT_SOURCE_DIR}/extern/license-generator")
	ENDIF(NOT olm_FOUND)
ENDIF(OLM_LOCATION)


