# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


include(CheckIncludeFile)

# Try to find DPDK
#
# Once done, this will define:
#  DPDK_FOUND            - System has DPDK
#  DPDK_INCLUDE_DIRS     - The DPDK include directories
#  DPDK_LIBRARIES        - The DPDK library
#  DPDK_PDUMP_AVAILABLE  - True when the optional DPDK pdump library is available

# DPDK requires PkgConfig
find_package(PkgConfig REQUIRED)

# Set search path
if (DEFINED ENV{DPDK_DIR})
    file(TO_CMAKE_PATH "$ENV{DPDK_DIR}" DPDK_SEARCH_PATH)
    set(CMAKE_PREFIX_PATH "${DPDK_SEARCH_PATH}")
endif (DEFINED ENV{DPDK_DIR})

# Find DPDK
unset(DPDK_FOUND CACHE)
pkg_check_modules(DPDK libdpdk>=${DPDK_MIN_VERSION})

if (DPDK_FOUND)
    # In case a specific path was provided, check that the library was not relocated after installation.
    if (DEFINED ENV{DPDK_DIR})
        if (NOT ${DPDK_PREFIX} STREQUAL ${DPDK_SEARCH_PATH})
            message(WARNING
                    "DPDK prefix detected by pkg-config (${DPDK_PREFIX}) does not match the requested DPDK path "
                    "(${DPDK_SEARCH_PATH}), the resulting libraries and headers path might be incorrect. "
                    "Please check whether the library was relocated from its original installation directory")
        endif()
    endif(DEFINED ENV{DPDK_DIR})
endif (DPDK_FOUND)

if (DPDK_FOUND)
    set(DPDK_LIBRARIES ${DPDK_LDFLAGS})
    message(STATUS "DPDK LIBRARIES: " ${DPDK_LIBRARIES})
    message(STATUS "DPDK INCLUDE DIRS: ${DPDK_INCLUDE_DIRS}")

    # The pdump library is optional in DPDK and its header is only installed when the library is built, so probe for
    # its availability.
    set(DPDK_PDUMP_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
    set(CMAKE_REQUIRED_INCLUDES ${DPDK_INCLUDE_DIRS})
    check_include_file(rte_pdump.h DPDK_PDUMP_AVAILABLE)
    set(CMAKE_REQUIRED_INCLUDES ${DPDK_PDUMP_CMAKE_REQUIRED_INCLUDES})
    unset(DPDK_PDUMP_CMAKE_REQUIRED_INCLUDES)
    if (DPDK_PDUMP_AVAILABLE)
        message(STATUS "DPDK pdump library: found")
    else ()
        message(STATUS "DPDK pdump library: not found, building without pdump support")
    endif ()
endif (DPDK_FOUND)
