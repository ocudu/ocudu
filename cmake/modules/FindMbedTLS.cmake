# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# - Try to find mbedtls
#
# Once done this will define
#  MBEDTLS_FOUND        - System has mbedtls
#  MBEDTLS_INCLUDE_DIRS - The mbedtls include directories
#  MBEDTLS_LIBRARIES    - The mbedtls library

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_MBEDTLS mbedtls)

if(NOT "$ENV{MBEDTLS_DIR}" STREQUAL "")
  set(_MBEDTLS_INCLUDE_HINTS "$ENV{MBEDTLS_DIR}/include")
  set(_MBEDTLS_LIB_HINTS     "$ENV{MBEDTLS_DIR}/lib")
endif()

#find Mbedtls
find_path(
    MBEDTLS_INCLUDE_DIRS
    NAMES mbedtls/md.h
    HINTS ${MBEDTLS_DIR}/include
          ${_MBEDTLS_INCLUDE_HINTS}
          ${PC_MBEDTLS_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

find_library(
    MBEDTLS_LIBRARIES
    NAMES mbedcrypto
    HINTS ${MBEDTLS_DIR}/lib
          ${_MBEDTLS_LIB_HINTS}
          ${PC_MBEDTLS_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

find_library(
    MBEDTLS_STATIC_LIBRARIES
    NAMES libmbedcrypto.a
    HINTS ${MBEDTLS_DIR}/lib
          ${_MBEDTLS_LIB_HINTS}
          ${PC_MBEDTLS_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

unset(_MBEDTLS_INCLUDE_HINTS)
unset(_MBEDTLS_LIB_HINTS)

message(STATUS "MBEDTLS LIBRARIES: " ${MBEDTLS_LIBRARIES})
message(STATUS "MBEDTLS STATIC LIBRARIES: " ${MBEDTLS_STATIC_LIBRARIES})
message(STATUS "MBEDTLS INCLUDE DIRS: " ${MBEDTLS_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MbedTLS DEFAULT_MSG MBEDTLS_LIBRARIES MBEDTLS_INCLUDE_DIRS)
mark_as_advanced(MBEDTLS_LIBRARIES MBEDTLS_STATIC_LIBRARIES MBEDTLS_INCLUDE_DIRS)
