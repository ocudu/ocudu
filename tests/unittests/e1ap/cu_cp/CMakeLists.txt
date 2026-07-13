# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(e1ap_cu_cp_test_helpers e1ap_cu_cp_test_helpers.cpp)
target_include_directories(e1ap_cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test_helpers e1ap_test_helpers ocudu_e1ap_cu_cp ocudu_pcap ocudu_support e1ap_asn1 ocudulog)

set(SOURCES
        e1ap_cu_cp_ue_context_test.cpp
        e1ap_cu_cp_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_modification_procedure_test.cpp
        e1ap_cu_cp_bearer_context_release_procedure_test.cpp
        e1ap_cu_cp_reset_test.cpp
        e1ap_cu_cp_test.cpp)

add_executable(e1ap_cu_cp_test ${SOURCES})
set_target_properties(e1ap_cu_cp_test PROPERTIES UNITY_BUILD ON)
target_include_directories(e1ap_cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test
        e1ap_cu_cp_test_helpers
        e1ap_test_helpers
        ocudu_pcap
        ocudu_support
        ocudulog
        e1ap_asn1
        GTest::gtest
        ocudu_gtest_main)

add_test(e1ap_cu_cp_test e1ap_cu_cp_test)
set_tests_properties(e1ap_cu_cp_test PROPERTIES LABELS "e1ap;e1ap_cu_cp")
