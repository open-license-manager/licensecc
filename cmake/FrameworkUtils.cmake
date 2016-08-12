
# This file is part of the LITIV framework; visit the original repository at
# https://github.com/plstcharles/litiv for more information.
#
# Copyright 2015 Pierre-Luc St-Charles; pierre-luc.st-charles<at>polymtl.ca
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

macro(xfix_list_tokens list_name prefix suffix)
    set(${list_name}_TMP)
    foreach(l ${${list_name}})
        list(APPEND ${list_name}_TMP ${prefix}${l}${suffix} )
    endforeach(l ${${list_name}})
    set(${list_name} "${${list_name}_TMP}")
    unset(${list_name}_TMP)
endmacro(xfix_list_tokens)

macro(append_internal_list list_name value)
    if(${list_name})
        set(${list_name} "${${list_name}};${value}" CACHE INTERNAL "Internal list variable")
    else(NOT ${list_name})
        set(${list_name} "${value}" CACHE INTERNAL "Internal list variable")
    endif()
endmacro(append_internal_list)

macro(initialize_internal_list list_name)
    set(${list_name} "" CACHE INTERNAL "Internal list variable")
endmacro(initialize_internal_list)

macro(litiv_module name)
    project(litiv_${name})
    append_internal_list(litiv_modules ${name})
    append_internal_list(litiv_projects litiv_${name})
    set(LITIV_CURRENT_MODULE_NAME ${name})
    set(LITIV_CURRENT_PROJECT_NAME litiv_${name})
endmacro(litiv_module)

macro(set_eval name)
    if(${ARGN})
        set(${name} 1)
    else(NOT ${ARGN})
        set(${name} 0)
    endif()
endmacro(set_eval)

macro(add_files list_name)
    foreach(filepath ${ARGN})
        list(APPEND ${list_name} "${filepath}")
    endforeach()
endmacro()

macro(target_link_litiv_dependencies name)
    target_link_libraries(${name} ${OpenCV_LIBRARIES})
    if(USE_GLSL)
        if(USE_GLFW)
            target_link_libraries(${name} ${GLFW_LIBRARIES})
        else(USE_FREEGLUT)
            target_link_libraries(${name} ${FREEGLUT_LIBRARY})
        endif()
        target_link_libraries(${name} ${OPENGL_LIBRARIES})
        target_link_libraries(${name} ${GLEW_LIBRARIES})
        target_link_libraries(${name} ${GLM_LIBRARIES})
    endif()
    if(USE_OPENGM)
        target_link_libraries(${name} ${OpenGM_LIBRARIES})
    endif()
    if(USE_CUDA)
        # @@@@ add cuda package
        message(FATAL_ERROR "Missing CUDA target link libraries")
    endif()
    if(USE_OPENCL)
        # @@@@ add opencl package
        message(FATAL_ERROR "Missing OpenCL target link libraries")
    endif()
endmacro(target_link_litiv_dependencies name)

macro(try_runcheck_and_set_success name description defaultval)
    if(NOT (DEFINED USE_${name}))
        if(CMAKE_CROSSCOMPILING) # all code intrinsics should be user-defined
            option(USE_${name} ${description} OFF)
        elseif(WIN32) # try_run often fails on windows due to linking/dll issues
            option(USE_${name} ${description} ${defaultval})
        else()
            message(STATUS "Testing local support for ${name} instructions via OpenCV...")
            try_run(${name}_RUN_RESULT ${name}_COMPILE_RESULT ${CMAKE_BINARY_DIR}/cmake/checks/ ${CMAKE_BINARY_DIR}/cmake/checks/${name}.cpp LINK_LIBRARIES ${OpenCV_LIBS} COMPILE_OUTPUT_VARIABLE ${name}_COMPILE_OUTPUT)
            set_eval(USE_${name} (${name}_RUN_RESULT AND ${name}_COMPILE_RESULT))
            option(USE_${name} ${description} ${USE_${name}})
            file(REMOVE ${CMAKE_BINARY_DIR}/cmake/checks/${name}.cpp)
        endif()
    endif()
endmacro(try_runcheck_and_set_success)

macro(try_cvhardwaresupport_runcheck_and_set_success name defaultval)
    set(CV_HARDWARE_SUPPORT_CHECK_FLAG_NAME ${name})
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/checks/cvhardwaresupport_check.cpp.in"
        "${CMAKE_BINARY_DIR}/cmake/checks/${name}.cpp"
    )
    try_runcheck_and_set_success(${name} "Allow implementations to use ${name} instructions" ${defaultval})
endmacro(try_cvhardwaresupport_runcheck_and_set_success)

macro(get_subdirectory_list result dir)
    file(GLOB children RELATIVE ${dir} ${dir}/*)
    set(dirlisttemp "")
    foreach(child ${children})
        if(IS_DIRECTORY ${dir}/${child})
            list(APPEND dirlisttemp ${child})
        endif()
    endforeach(child ${children})
    set(${result} ${dirlisttemp})
endmacro(get_subdirectory_list result dir)
