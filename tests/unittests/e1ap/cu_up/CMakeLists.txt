# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(e1ap_cu_up_test_helpers e1ap_cu_up_test_helpers.cpp)
target_include_directories(e1ap_cu_up_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_up_test_helpers e1ap_test_helpers ocudu_e1ap_cu_up ocudu_pcap ocudu_support e1ap_asn1 ocudulog)

set(SOURCES
        e1ap_cu_up_test.cpp)

add_executable(e1ap_cu_up_test ${SOURCES})
target_include_directories(e1ap_cu_up_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_up_test e1ap_cu_up_test_helpers e1ap_test_helpers ocudu_e1ap_cu_up ocudu_pcap ocudu_support ocudulog e1ap_asn1 GTest::gtest GTest::gtest_main)
gtest_discover_tests(e1ap_cu_up_test PROPERTIES "LABELS;e1ap_cu_up")
