# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "gateways")

include_directories(../../..)

add_executable(udp_network_gateway_benchmark udp_network_gateway_benchmark.cpp)
target_link_libraries(udp_network_gateway_benchmark ocudu_gateway ocudu_support ocudulog)

add_executable(udp_network_gateway_tx_benchmark udp_network_gateway_tx_benchmark.cpp)
target_link_libraries(udp_network_gateway_tx_benchmark ocudu_gateway ocudu_support ocudulog)

add_executable(udp_network_gateway_rx_benchmark udp_network_gateway_rx_benchmark.cpp)
target_link_libraries(udp_network_gateway_rx_benchmark ocudu_gateway ocudu_support ocudulog)
