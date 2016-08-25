#
# FindMetaLic.cmake
#
# This module defines the following variables:
#   MetaLic_INCLUDE_DIRS
#   MetaLic_GENERATOR_LIBRARIES
#   MetaLic_READER_LIBRARIES
#   MetaLic_FOUND
#
# You can help it find the required paths/files by pre-setting
# the 'MetaLic_DIR' variable to the right install directory.
#

include(FindPackageHandleStandardArgs)

find_path(MetaLic_INCLUDE_DIR
    NAMES
        "metalicensor/keys/public-key.h"
    HINTS
        "${MetaLic_DIR}/include/"
        "${MetaLic_DIR}/build/include/"
        "${MetaLic_DIR}/build/install/include/"
        "${MetaLic_ROOT_DIR}/include/"
        "${MetaLic_ROOT_DIR}/build/include/"
        "${MetaLic_ROOT_DIR}/build/install/include/"
        "$ENV{METALIC_ROOT}/include/"
        "$ENV{METALIC_ROOT}/build/include/"
        "$ENV{METALIC_ROOT}/build/install/include/"
        "$ENV{USER_DEVELOP}/MetaLic/include/"
        "$ENV{USER_DEVELOP}/vendor/MetaLic/include/"
        "$ENV{USER_DEVELOP}/metalicensor/include/"
        "$ENV{USER_DEVELOP}/vendor/metalicensor/include/"
)

find_library(MetaLic_GENERATOR_LIBRARY
    NAMES
        "metalic_generator"
    HINTS
        "${MetaLic_DIR}/lib/"
        "${MetaLic_DIR}/build/lib/"
        "${MetaLic_DIR}/build/install/lib/"
        "${MetaLic_ROOT_DIR}/lib/"
        "${MetaLic_ROOT_DIR}/build/lib/"
        "${MetaLic_ROOT_DIR}/build/install/lib/"
        "$ENV{METALIC_ROOT}/lib/"
        "$ENV{METALIC_ROOT}/build/lib/"
        "$ENV{METALIC_ROOT}/build/install/lib/"
        "$ENV{USER_DEVELOP}/MetaLic/lib/"
        "$ENV{USER_DEVELOP}/vendor/MetaLic/lib/"
        "$ENV{USER_DEVELOP}/metalicensor/lib/"
        "$ENV{USER_DEVELOP}/vendor/metalicensor/lib/"
        "${MetaLic_INCLUDE_DIR}/../lib/"
        "${MetaLic_LIBRARY_DIR}"
)

find_library(MetaLic_READER_LIBRARY
    NAMES
        "metalic_reader"
    HINTS
        "${MetaLic_DIR}/lib/"
        "${MetaLic_DIR}/build/lib/"
        "${MetaLic_DIR}/build/install/lib/"
        "${MetaLic_ROOT_DIR}/lib/"
        "${MetaLic_ROOT_DIR}/build/lib/"
        "${MetaLic_ROOT_DIR}/build/install/lib/"
        "$ENV{METALIC_ROOT}/lib/"
        "$ENV{METALIC_ROOT}/build/lib/"
        "$ENV{METALIC_ROOT}/build/install/lib/"
        "$ENV{USER_DEVELOP}/MetaLic/lib/"
        "$ENV{USER_DEVELOP}/vendor/MetaLic/lib/"
        "$ENV{USER_DEVELOP}/metalicensor/lib/"
        "$ENV{USER_DEVELOP}/vendor/metalicensor/lib/"
        "${MetaLic_INCLUDE_DIR}/../lib/"
        "${MetaLic_LIBRARY_DIR}"
)

mark_as_advanced(
    MetaLic_INCLUDE_DIR
    MetaLic_GENERATOR_LIBRARY
    MetaLic_READER_LIBRARY
)
if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    find_package(ZLIB REQUIRED) # Zlib required when openssl version < 1.0.1f
endif(NOT WIN32)
set(MetaLic_DIR_old ${MetaLic_DIR})
set(MetaLic_DIR "MetaLic_DIR-NOTFOUND" CACHE PATH "The directory to the MetaLicensor installation, i.e. where include and lib directories can be found." FORCE)
find_package_handle_standard_args(MetaLic
    REQUIRED_VARS
        MetaLic_INCLUDE_DIR
        MetaLic_GENERATOR_LIBRARY
        MetaLic_READER_LIBRARY
)
if(MetaLic_FOUND)
    set(MetaLic_DIR "${MetaLic_DIR_old}" CACHE PATH "The directory to the MetaLicensor installation, i.e. where include and lib directories can be found." FORCE)
    set(MetaLic_INCLUDE_DIRS ${MetaLic_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
    set(MetaLic_GENERATOR_LIBRARIES ${MetaLic_GENERATOR_LIBRARY})
    set(MetaLic_READER_LIBRARIES ${MetaLic_READER_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ZLIB_LIBRARIES} dl)
    mark_as_advanced(MetaLic_INCLUDE_DIRS MetaLic_GENERATOR_LIBRARIES MetaLic_READER_LIBRARIES)
endif()
