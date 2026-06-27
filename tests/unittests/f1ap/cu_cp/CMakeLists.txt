# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(f1ap_cu_test_helpers f1ap_cu_test_helpers.cpp)
target_include_directories(f1ap_cu_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_cu_test_helpers ocudu_f1ap_du ocudu_f1ap_cu ocudu_support f1ap_asn1 ocudulog)

set(SOURCES
        f1ap_cu_gnb_cu_configuration_update_procedure_test.cpp
        f1ap_cu_positioning_measurement_procedure_test.cpp
        f1ap_cu_trp_information_exchange_procedure_test.cpp
        f1ap_cu_ue_context_test.cpp
        f1ap_cu_ue_context_setup_procedure_test.cpp
        f1ap_cu_ue_context_modification_procedure_test.cpp
        f1ap_cu_ue_context_release_procedure_test.cpp
        f1ap_cu_test.cpp
        f1ap_cu_paging_test.cpp
        f1ap_cu_write_replace_warning_test.cpp
        f1ap_cu_ref_time_info_report_test.cpp)

add_executable(f1ap_cu_test ${SOURCES})
target_include_directories(f1ap_cu_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_cu_test f1ap_cu_test_helpers f1ap_test_doubles GTest::gtest ocudu_gtest_main)
gtest_discover_tests(f1ap_cu_test PROPERTIES "LABELS;f1ap_cu")
