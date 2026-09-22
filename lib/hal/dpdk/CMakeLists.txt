# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_subdirectory(bbdev)

set(SOURCES
  dpdk_eal.cpp
  dpdk_eal_factory.cpp
  dpdk.cpp
  mbuf_pool_factory.cpp
)

add_library(hal_dpdk STATIC ${SOURCES})
target_compile_options(hal_dpdk PRIVATE ${DPDK_CFLAGS})
target_link_libraries(hal_dpdk ocudulog ${DPDK_LIBRARIES})

if (DPDK_PDUMP_AVAILABLE)
    target_compile_definitions(hal_dpdk PRIVATE DPDK_PDUMP_AVAILABLE)
endif (DPDK_PDUMP_AVAILABLE)

add_to_exported_libs(hal_dpdk)
