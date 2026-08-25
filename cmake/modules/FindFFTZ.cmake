# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Try to find the AMD Optimized FFT for Zen (AOCL-FFTZ) library.
# Once done this will define
#  FFTZ_FOUND - System has FFTZ
#  FFTZ_INCLUDE_DIRS - The FFTZ include directories
#  FFTZ_LIBRARIES - The libraries needed to use FFTZ

find_path(FFTZ_INCLUDE_DIR
        NAMES aoclfftz.h
        HINTS $ENV{FFTZ_DIR}/include/
        PATHS /usr/local/include/)

find_library(FFTZ_LIBRARY_STATIC
        NAMES libaocl_fftz.a
        HINTS $ENV{FFTZ_DIR}/lib/
        PATHS /usr/local/lib/)

find_library(FFTZ_LIBRARY_SHARED
        NAMES libaocl_fftz.so
        HINTS $ENV{FFTZ_DIR}/lib/
        PATHS /usr/local/lib/)

if (FFTZ_LIBRARY_STATIC)
    set(FFTZ_LIBRARIES ${FFTZ_LIBRARY_STATIC})
elseif (FFTZ_LIBRARY_SHARED)
    set(FFTZ_LIBRARIES ${FFTZ_LIBRARY_SHARED})
endif()
set(FFTZ_INCLUDE_DIRS ${FFTZ_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

# Handle the QUIETLY and REQUIRED arguments and set FFTZ_FOUND to TRUE
# if all listed variables are TRUE.
find_package_handle_standard_args(FFTZ DEFAULT_MSG FFTZ_INCLUDE_DIR FFTZ_LIBRARIES)

if (FFTZ_FOUND)
    MESSAGE(STATUS "Found FFTZ INCLUDE DIRS: ${FFTZ_INCLUDE_DIRS}")
    MESSAGE(STATUS "Found FFTZ LIBRARIES: ${FFTZ_LIBRARIES}")
endif (FFTZ_FOUND)

mark_as_advanced(FFTZ_INCLUDE_DIR FFTZ_LIBRARIES)
