# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# - Try to find IPSEC_MB library
#
# Once done this will define
#  IPSEC_MB_FOUND        - System has IPSEC_MB library installed
#  IPSEC_MB_INCLUDE_DIRS - The IPSEC_MB include directories
#  IPSEC_MB_LIBRARIES    - The IPSEC_MB library

#find IPSec_MB
find_path(
    IPSEC_MB_INCLUDE_DIRS
    NAMES ipsec-mb.h
    HINTS ${IPSEC_MB_DIR}/include
          $ENV{IPSEC_MB_DIR}/include
          ${PC_IPSEC_MB_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

find_library(
	IPSEC_MB_LIBRARIES
    NAMES IPSec_MB
    HINTS ${IPSEC_MB_DIR}/lib
          $ENV{IPSEC_MB_DIR}/lib
          ${PC_IPSEC_MB_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IPSecMB DEFAULT_MSG IPSEC_MB_LIBRARIES IPSEC_MB_INCLUDE_DIRS)
mark_as_advanced(IPSEC_MB_LIBRARIES IPSEC_MB_INCLUDE_DIRS)

if(IPSEC_MB_FOUND)
	message(STATUS "IPSEC_MB LIBRARIES: " ${IPSEC_MB_LIBRARIES})
	message(STATUS "IPSEC_MB INCLUDE DIRS: " ${IPSEC_MB_INCLUDE_DIRS})
endif()
