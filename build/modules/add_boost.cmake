

#usage add_boost(	[STATIC] , 
#					[STATIC_RUNTIME], 
#					[USE_BOOST_CACHE], #If user wants to use a cache copy of Boost, get the path to this location.
#					[BOOST_CACHE_DIR path],
#					[TARGET_ARCHITECHTURE_32],
#					[MODULES module1, module2] )

#This module will set the following variables
#   Boost_INCLUDE_DIR      - Boost include directories
#   Boost_LIBRARY_DIR      - Link directories for Boost libraries
#   Boost_LIBRARIES        - Boost component libraries to be linked
#   Boost_<C>_LIBRARY      - Libraries to link for component <C> (c is lower-case,may include
#                            target_link_libraries debug/optimized keywords)

include (CMakeParseArguments)
	
function(add_boost)
	set(BoostVersion 1.55.0)
	set(BoostSHA1 cef9a0cc7084b1d639e06cd3bc34e4251524c840)

	set(options STATIC STATIC_RUNTIME USE_BOOST_CACHE TARGET_ARCHITECHTURE_32)
    set(oneValueArgs BOOST_CACHE_DIR)
    set(multiValueArgs MODULES)
    cmake_parse_arguments(ADD_BOOST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
	
	# Create build folder name derived from version
	string(REGEX REPLACE "beta\\.([0-9])$" "beta\\1" BoostFolderName ${BoostVersion})
	string(REPLACE "." "_" BoostFolderName ${BoostFolderName})
	set(BoostFolderName boost_${BoostFolderName})

	# If user wants to use a cache copy of Boost, get the path to this location.
	#ADD_BOOST_USE_BOOST_CACHE is a parsed command line parameter
	if(ADD_BOOST_USE_BOOST_CACHE)
	  if(ADD_BOOST_BOOST_CACHE_DIR)
		file(TO_CMAKE_PATH "${BOOST_CACHE_DIR}" BoostCacheDir)
	  elseif(WIN32)
		ms_get_temp_dir()
		set(BoostCacheDir "${TempDir}")
	  elseif(APPLE)
		set(BoostCacheDir "$ENV{HOME}/Library/Caches")
	  else()
		set(BoostCacheDir "$ENV{HOME}/.cache")
	  endif()
	endif()

	# If the cache directory doesn't exist, fall back to use the build root.
	if(NOT IS_DIRECTORY "${BoostCacheDir}")
	  if(ADD_BOOST_BOOST_CACHE_DIR)
		set(Message "\nThe directory \"${ADD_BOOST_BOOST_CACHE_DIR}\" provided in BOOST_CACHE_DIR doesn't exist.")
		set(Message "${Message}  Falling back to default path at \"${CMAKE_BINARY_DIR}/license++\"\n")
		message(WARNING "${Message}")
	  endif()
	  set(BoostCacheDir ${CMAKE_BINARY_DIR})
	else()
	  if(NOT ADD_BOOST_USE_BOOST_CACHE AND NOT ADD_BOOST_BOOST_CACHE_DIR)
		set(BoostCacheDir "${BoostCacheDir}/license++")
	  endif()
	  file(MAKE_DIRECTORY "${BoostCacheDir}")
	endif()

	# Set up the full path to the source directory
	set(BoostSourceDir "${BoostFolderName}_${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
	if(HAVE_LIBC++)
	  set(BoostSourceDir "${BoostSourceDir}_LibCXX")
	endif()
	if(HAVE_LIBC++ABI)
	  set(BoostSourceDir "${BoostSourceDir}_LibCXXABI")
	endif()
	if(CMAKE_CL_64)
	  set(BoostSourceDir "${BoostSourceDir}_Win64")
	endif()
	string(REPLACE "." "_" BoostSourceDir ${BoostSourceDir})
	set(BOOST_ROOT "${BoostCacheDir}/${BoostSourceDir}" CACHE PATH "BOOST base dir")

	# Check the full path to the source directory is not too long for Windows.  File paths must be less
	# than MAX_PATH which is 260.  The current longest relative path Boost tries to create is:
	# Build\boost\bin.v2\libs\program_options\build\fd41f4c7d882e24faa6837508d6e5384\libboost_program_options-vc120-mt-gd-1_55.lib.rsp
	# which along with a leading separator is 129 chars in length.  This gives a maximum path available
	# for 'BOOST_ROOT' as 130 chars.
	if(WIN32)
	  get_filename_component(BoostSourceDirName "${BOOST_ROOT}" NAME)
	  string(LENGTH "/${BoostSourceDirName}" BoostSourceDirNameLengthWithSeparator)
	  math(EXPR AvailableLength 130-${BoostSourceDirNameLengthWithSeparator})
	  string(LENGTH "${BOOST_ROOT}" BoostSourceDirLength)
	  if(${BoostSourceDirLength} GREATER 130)
		set(Msg "\n\nThe path to boost's source is too long to handle all the files which will ")
		set(Msg "${Msg}be created when boost is built.  To avoid this, set the CMake variable ")
		set(Msg "${Msg}USE_BOOST_CACHE to ON and set the variable BOOST_CACHE_DIR to a path ")
		set(Msg "${Msg}which is at most ${AvailableLength} characters long.  For example:\n")
		set(Msg "${Msg}  mkdir C:\\license_boost\n")
		set(Msg "${Msg}  cmake . -DUSE_BOOST_CACHE=ON -DBOOST_CACHE_DIR=C:\\license_boost\n\n")
		message(FATAL_ERROR "${Msg}")
	  endif()
	endif()

	# Download boost if required
	set(ZipFilePath "${BoostCacheDir}/${BoostFolderName}.tar.bz2")
	if(NOT EXISTS ${ZipFilePath})
	  message(STATUS "Downloading boost ${BoostVersion} to ${BoostCacheDir}")
	endif()
	file(DOWNLOAD http://sourceforge.net/projects/boost/files/boost/${BoostVersion}/${BoostFolderName}.tar.bz2/download
		 ${ZipFilePath}
		 STATUS Status
		 SHOW_PROGRESS
		 EXPECTED_HASH SHA1=${BoostSHA1}
		 )

	# Extract boost if required
	string(FIND "${Status}" "returning early" Found)
	if(Found LESS 0 OR NOT IS_DIRECTORY "${BOOST_ROOT}")
	  set(BoostExtractFolder "${BoostCacheDir}/boost_unzip")
	  file(REMOVE_RECURSE ${BoostExtractFolder})
	  file(MAKE_DIRECTORY ${BoostExtractFolder})
	  file(COPY ${ZipFilePath} DESTINATION ${BoostExtractFolder})
	  message(STATUS "Extracting boost ${BoostVersion} to ${BoostExtractFolder}")
	  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${BoostFolderName}.tar.bz2
					  WORKING_DIRECTORY ${BoostExtractFolder}
					  RESULT_VARIABLE Result
					  )
	  if(NOT Result EQUAL 0)
		message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostExtractFolder}")
	  endif()
	  file(REMOVE ${BoostExtractFolder}/${BoostFolderName}.tar.bz2)

	  # Get the path to the extracted folder
	  file(GLOB ExtractedDir "${BoostExtractFolder}/*")
	  list(LENGTH ExtractedDir n)
	  if(NOT n EQUAL 1 OR NOT IS_DIRECTORY ${ExtractedDir})
		message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostExtractFolder}")
	  endif()
	  file(RENAME ${ExtractedDir} ${BOOST_ROOT})
	  file(REMOVE_RECURSE ${BoostExtractFolder})
	endif()


	# Build b2 (bjam) if required
	find_program(b2Path NAMES b2 PATHS ${BOOST_ROOT} NO_DEFAULT_PATH)
	if(NOT b2Path)
	  message(STATUS "Building b2 (bjam)")
	  if(MSVC)
		set(b2Bootstrap "bootstrap.bat")
	  else()
		set(b2Bootstrap "./bootstrap.sh")
		if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
		  list(APPEND b2Bootstrap --with-toolset=clang)
		elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		  list(APPEND b2Bootstrap --with-toolset=gcc)
		endif()
	  endif()
	  execute_process(COMMAND ${b2Bootstrap} WORKING_DIRECTORY ${BOOST_ROOT}
					  RESULT_VARIABLE Result OUTPUT_VARIABLE Output ERROR_VARIABLE Error)
	  find_program(b2Path NAMES b2 PATHS ${BOOST_ROOT} NO_DEFAULT_PATH)
	  if(NOT Result EQUAL 0 OR NOT b2Path)
		message(FATAL_ERROR "Failed running ${b2Bootstrap}:\n${Output}\n${Error}\n")
	  endif()
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BOOST_ROOT}/Build)

	#ADD_BOOST_STATIC_RUNTIME is a parsed function parameter
	if(ADD_BOOST_STATIC_RUNTIME)
		set(RUNTIME_LINK "static")
		set(BOOST_LIB_SUFFIX "-s")
		set(Boost_USE_STATIC_RUNTIME ON)
	else()
		set(RUNTIME_LINK "shared")
		set(BOOST_LIB_SUFFIX "")
		set(Boost_USE_STATIC_RUNTIME OFF)
	endif()
	#ADD_BOOST_STATIC is a parsed function parameter
	if(ADD_BOOST_STATIC)
		set(BOOST_LINK "static")
		set(LINUX_LIB_EXTENSION "a")
		set(WINDOWS_LIB_PREFIX "lib")
		set(Boost_USE_STATIC_LIBS ON)
	else(ADD_BOOST_STATIC)
		set(BOOST_LINK "shared")
		set(LINUX_LIB_EXTENSION "so")
		set(WINDOWS_LIB_PREFIX "")
		set(Boost_USE_STATIC_LIBS OFF)
	endif(ADD_BOOST_STATIC)
	message(STATUS "Boost linking: libs ${BOOST_LINK} , C++ runtime ${RUNTIME_LINK}")
	# Set up general b2 (bjam) command line arguments
	set(b2Args ./b2
			   link=${BOOST_LINK}
			   threading=multi
			   runtime-link=${RUNTIME_LINK}
			   --build-dir=Build
			   stage
			   -d+2
			   --hash
			   )
	if("${CMAKE_BUILD_TYPE}" STREQUAL "ReleaseNoInline")
	  list(APPEND b2Args cxxflags="${RELEASENOINLINE_FLAGS}")
	endif()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "DebugLibStdcxx")
	  list(APPEND b2Args define=_GLIBCXX_DEBUG)
	endif()

	# Set up platform-specific b2 (bjam) command line arguments
	if(MSVC)
	  if(MSVC11)
		list(APPEND b2Args toolset=msvc-11.0)
	  elseif(MSVC12)
		list(APPEND b2Args toolset=msvc-12.0)
	  endif()
	  list(APPEND b2Args
				  define=_BIND_TO_CURRENT_MFC_VERSION=1
				  define=_BIND_TO_CURRENT_CRT_VERSION=1
				  --layout=versioned
				  )
		#command line parameter
		if(NOT ADD_BOOST_TARGET_ARCHITECHTURE_32)
			list(APPEND b2Args address-model=64)
		endif()
	elseif(APPLE)
		list(APPEND b2Args variant=release toolset=clang cxxflags=-fPIC cxxflags=-std=c++11 cxxflags=-stdlib=libc++
						 linkflags=-stdlib=libc++ architecture=combined address-model=32_64 --layout=tagged)
	elseif(UNIX)
	  list(APPEND b2Args variant=release cxxflags=-fPIC cxxflags=-std=c++11 -sNO_BZIP2=1 --layout=tagged)
	  # Need to configure the toolset based on whatever version CMAKE_CXX_COMPILER is
	  message(STATUS "cmake compiler version ${CMAKE_CXX_COMPILER_VERSION}")
	  string(REGEX MATCH "[0-9]+\\.[0-9]+" ToolsetVer "${CMAKE_CXX_COMPILER_VERSION}")
	  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
		list(APPEND b2Args toolset=clang-${ToolsetVer})
		if(HAVE_LIBC++)
		  list(APPEND b2Args cxxflags=-stdlib=libc++ linkflags=-stdlib=libc++)
		endif()
	  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		list(APPEND b2Args toolset=gcc-${ToolsetVer})
	  endif()
	endif()

	if(NOT ADD_BOOST_MODULES)
		set (ADD_BOOST_MODULES atomic chrono context coroutine date_time exception filesystem
				graph iostreams log math program_options python random regex signals system 
				test thread timer wave)
	endif()
	
	foreach(Component ${ADD_BOOST_MODULES})
	  if(${Component} STREQUAL "test")
		set(libName "unit_test_framework")
	  elseif(${Component} STREQUAL "math")
		set(libName "math_c99l")
	  else()	
		set(libName ${Component})
	  endif()
	  if(MSVC)
		if(MSVC11)
		  set(CompilerName vc110)
		elseif(MSVC12)
		  set(CompilerName vc120)
		endif()
		string(REGEX MATCH "[0-9]_[0-9][0-9]" Version "${BoostFolderName}")	
		#http://www.boost.org/doc/libs/1_42_0/more/getting_started/windows.html#library-naming
		set(OUTPUT_FILE ${BOOST_ROOT}/stage/lib/${WINDOWS_LIB_PREFIX}boost_${libName}-${CompilerName}-mt-${Version}.lib)
		set(OUTPUT_FILE_DEBUG ${BOOST_ROOT}/stage/lib/${WINDOWS_LIB_PREFIX}boost_${libName}-${CompilerName}-mt-gd-${Version}.lib)
	  else(MSVC)
			set(OUTPUT_FILE ${BOOST_ROOT}/stage/lib/libboost_${libName}-mt${BOOST_LIB_SUFFIX}.${LINUX_LIB_EXTENSION})
	  endif(MSVC)
	  #if(NOT EXISTS "${OUTPUT_FILE}")
		  message(STATUS "Building ${Component}: ${b2Args}")
		  execute_process(COMMAND ${b2Args} --with-${Component} WORKING_DIRECTORY ${BOOST_ROOT} RESULT_VARIABLE Result OUTPUT_VARIABLE Output ERROR_VARIABLE Error)
		  if(NOT Result EQUAL 0)
			message(ERROR "Failed running ${b2Args} --with-${Component}:\n${Output}\n${Error}\n")
		  endif()
	  #endif()
	endforeach()
	
	
	LIST_REPLACE(ADD_BOOST_MODULES "test" "unit_test_framework")   
	#   BOOST_ROOT             - Preferred installation prefix
	#    (or BOOSTROOT)
	#   BOOST_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
	#   BOOST_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib
	#   Boost_NO_SYSTEM_PATHS  - Set to ON to disable searching in locations not
	#                            specified by these hint variables. Default is OFF.
	set (BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib CACHE PATH "BOOST library dir")
	#set (Boost_NO_SYSTEM_PATHS ON)
	find_package(Boost 1.55 REQUIRED COMPONENTS ${ADD_BOOST_MODULES})
	#clean up variables
	mark_as_advanced (BOOST_ROOT BOOST_LIBRARYDIR)
	unset(b2Path CACHE)
	
	set(Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE)
endfunction()

macro(LIST_REPLACE LIST OLDVALUE NEWVALUE)
    list(FIND ADD_BOOST_MODULES ${OLDVALUE} find_idx)                                    
    if(find_idx GREATER -1)                                                    
		list(INSERT ${LIST} ${find_idx} ${NEWVALUE})
		MATH(EXPR __INDEX "${find_idx} + 1")
		list (REMOVE_AT ${LIST} ${__INDEX})
	endif()       
endmacro(LIST_REPLACE)                          
