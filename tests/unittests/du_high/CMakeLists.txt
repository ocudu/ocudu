# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "du_high")

include_directories(../../..)

add_executable(du_high_clock_controller_test du_high_clock_controller_test.cpp)
target_link_libraries(du_high_clock_controller_test ocudu_du_high ocudu_support ocudulog GTest::gtest ocudu_gtest_main)
add_test(du_high_clock_controller_test du_high_clock_controller_test)

add_executable(f1ap_ref_time_provider_adapter_test f1ap_ref_time_provider_adapter_test.cpp)
target_link_libraries(f1ap_ref_time_provider_adapter_test ocudu_du_high ocudu_support GTest::gtest ocudu_gtest_main)
add_test(f1ap_ref_time_provider_adapter_test f1ap_ref_time_provider_adapter_test)
