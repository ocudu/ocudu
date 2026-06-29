# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


include_directories(../../..)

add_executable(rlc_handle_status_report rlc_handle_status_report.cpp)
target_link_libraries(rlc_handle_status_report ocudu_rlc ocudulog pdcp_test_doubles)
add_test(rlc_handle_status_report rlc_handle_status_report)

add_executable(rlc_am_rx_benchmark rlc_am_rx_benchmark.cpp)
target_link_libraries(rlc_am_rx_benchmark ocudu_rlc ocudulog pdcp_test_doubles)
add_test(rlc_am_rx_benchmark rlc_am_rx_benchmark)

add_executable(rlc_am_tx_benchmark rlc_am_tx_benchmark.cpp)
target_link_libraries(rlc_am_tx_benchmark ocudu_rlc ocudulog pdcp_test_doubles)
add_test(rlc_am_tx_benchmark rlc_am_tx_benchmark)

add_executable(rlc_instances rlc_instances.cpp)
target_link_libraries(rlc_instances ocudu_rlc ocudulog)
add_test(rlc_instances rlc_instances)

set_tests_properties(
  rlc_am_tx_benchmark
  rlc_am_rx_benchmark
  PROPERTIES LABELS "rlc|rtsan"
)

set_tests_properties(
  rlc_handle_status_report
  rlc_instances
  PROPERTIES LABELS "rlc"
)
