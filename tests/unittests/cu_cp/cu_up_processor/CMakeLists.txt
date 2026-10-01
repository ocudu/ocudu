# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(cu_up_processor_test_helpers cu_up_processor_test_helpers.cpp)
target_include_directories(cu_up_processor_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_up_processor_test_helpers cu_cp_test_helpers ocudu_cu_cp e1ap_test_helpers rrc_ue_test_helpers ocudu_support ngap_asn1 f1ap_asn1 ocudulog)

set(SOURCES
        cu_up_processor_test.cpp
        )

add_executable(cu_up_processor_test ${SOURCES})
target_include_directories(cu_up_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_up_processor_test cu_up_processor_test_helpers ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(cu_up_processor_test PROPERTIES "LABELS;cu_up_processor")
