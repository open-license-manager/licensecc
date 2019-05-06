#cmake_minimum_required(VERSION 3.0)
cmake_minimum_required(VERSION 2.8.11)

link_directories (   "${CMAKE_CURRENT_SOURCE_DIR}/../install/lib" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../install/include" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(MSVC)
SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib )
else(MSVC)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s -Wl,--exclude-libs,liblicense++_static.a")
find_package(OpenSSL REQUIRED)
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(example example.cpp)

target_link_libraries(example license++_static os base tools_base)

if(NOT MSVC)
target_link_libraries(example crypto pthread dl z)
endif(NOT MSVC)

if(MINGW)
target_link_libraries(example iphlpapi)
endif(MINGW)
