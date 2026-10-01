# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "pdcp")

include_directories(../../..)

add_executable(pdcp_tx_benchmark pdcp_tx_benchmark.cpp)
target_link_libraries(pdcp_tx_benchmark ocudu_pdcp ocudu_security ocudulog)
add_test(pdcp_tx_benchmark_nea0 pdcp_tx_benchmark -a0 -R3)
add_test(pdcp_tx_benchmark_nea1 pdcp_tx_benchmark -a1 -R1)
add_test(pdcp_tx_benchmark_nea2 pdcp_tx_benchmark -a2 -R3)
add_test(pdcp_tx_benchmark_nea3 pdcp_tx_benchmark -a3 -R1)
set_tests_properties(pdcp_tx_benchmark_nea0 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea1 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea2 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea3 PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_benchmark pdcp_rx_benchmark.cpp)
target_link_libraries(pdcp_rx_benchmark ocudu_pdcp ocudu_security ocudulog)
add_test(pdcp_rx_benchmark_nea0 pdcp_rx_benchmark -a0 -R3)
add_test(pdcp_rx_benchmark_nea1 pdcp_rx_benchmark -a1 -R1)
add_test(pdcp_rx_benchmark_nea2 pdcp_rx_benchmark -a2 -R3)
add_test(pdcp_rx_benchmark_nea3 pdcp_rx_benchmark -a3 -R1)
set_tests_properties(pdcp_rx_benchmark_nea0 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea1 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea2 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea3 PROPERTIES LABELS "tsan")
