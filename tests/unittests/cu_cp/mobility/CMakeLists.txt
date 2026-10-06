# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(mobility_test_helpers mobility_test_helpers.cpp ../du_processor_test_messages.cpp ../test_helpers.cpp)
target_include_directories(mobility_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mobility_test_helpers ocudu_cu_cp e1ap_test_helpers rrc_ue_test_helpers ngap_test_helpers ocudu_support f1ap_asn1 ocudulog)

set(SOURCES handover_reconfiguration_routine_test.cpp
            mobility_manager_helpers_test.cpp)

add_executable(cu_cp_mobility_test ${SOURCES})
target_include_directories(cu_cp_mobility_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_mobility_test cu_cp_test_helpers mobility_test_helpers f1ap_test_doubles ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(cu_cp_mobility_test PROPERTIES "LABELS;mobility")
