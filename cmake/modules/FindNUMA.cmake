# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


find_package(PkgConfig REQUIRED)

find_path(
    NUMA_INCLUDE_DIRS
    NAMES numa.h numaif.h
    PATHS /usr/local/include
    /usr/include
)

find_library(
        NUMA_LIBRARIES
        NAMES numa
        PATHS /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NUMA DEFAULT_MSG NUMA_LIBRARIES NUMA_INCLUDE_DIRS)
MARK_AS_ADVANCED(NUMA_LIBRARIES NUMA_INCLUDE_DIRS)
