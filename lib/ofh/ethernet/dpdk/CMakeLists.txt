# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        dpdk_ethernet_port_context.cpp
        dpdk_ethernet_transmitter.cpp
        dpdk_ethernet_factories.cpp
        dpdk_ethernet_receiver.cpp)

add_library(ocudu_ofh_dpdk_ethernet STATIC ${SOURCES})
target_compile_options(ocudu_ofh_dpdk_ethernet PRIVATE ${DPDK_CFLAGS})
target_link_libraries(ocudu_ofh_dpdk_ethernet ocudulog ocudu_instrumentation ${DPDK_LIBRARIES})
