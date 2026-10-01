# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "du_high|tsan|rtsan")

include_directories(../../..)

add_executable(du_high_benchmark du_high_benchmark.cpp)
target_link_libraries(du_high_benchmark
  ocudu_du_high
  f1ap_du_test_helpers
  pdcp_test_doubles
  sched_test_doubles
  mac_test_doubles
  du_test_doubles
  ocudu_pcap
  gtest
)
add_test(du_high_benchmark du_high_benchmark)
