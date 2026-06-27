# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "rrc")

add_library(rrc_ue_test_helpers rrc_ue_test_messages.cpp)
target_include_directories(rrc_ue_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test_helpers ocudu_support ocudulog)

set(SOURCES
        rrc_asn1_helpers_test.cpp
        rrc_du_ref_time_r16_test.cpp
        rrc_ue_setup_proc_test.cpp
        rrc_ue_dl_info_transfer_proc_test.cpp
        rrc_ue_reconfig_proc_test.cpp
        rrc_ue_capability_transfer_proc_test.cpp
        rrc_ue_reest_proc_test.cpp
        rrc_ue_meas_report_test.cpp)

add_executable(rrc_ue_test ${SOURCES})
target_include_directories(rrc_ue_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test rrc_ue_test_helpers f1ap_test_doubles ocudu_rrc ocudu_cu_cp ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(rrc_ue_test PROPERTIES "LABELS;rrc_ue")

add_executable(rrc_cond_trigger_asn1_test rrc_cond_trigger_asn1_test.cpp)
target_include_directories(rrc_cond_trigger_asn1_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_cond_trigger_asn1_test ocudu_rrc ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(rrc_cond_trigger_asn1_test PROPERTIES "LABELS;rrc_ue")
