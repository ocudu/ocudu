# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(du_processor_test_helpers du_processor_test_helpers.cpp)
target_include_directories(du_processor_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_processor_test_helpers
        cu_cp_test_helpers
        ocudu_cu_cp
        e1ap_test_helpers
        ngap_test_helpers
        rrc_ue_test_helpers
        f1ap_test_doubles
        ocudu_support
        f1ap_asn1
        ocudulog)

set(SOURCES
        du_configuration_manager_test.cpp
        du_processor_test.cpp
        )

add_executable(du_processor_test ${SOURCES})
target_include_directories(du_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_processor_test du_processor_test_helpers ocudu_support ocudu_e2 ocudulog GTest::gtest ocudu_gtest_main)
add_test(du_processor_test du_processor_test)
set_tests_properties(du_processor_test PROPERTIES LABELS "LABELS;du_processor")
gtest_discover_tests(du_processor_test PROPERTIES "LABELS;du_processor")
