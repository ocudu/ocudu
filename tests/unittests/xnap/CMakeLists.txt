# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(xnap_test_helpers xnap_test_helpers.cpp xnap_test_messages.cpp)
target_include_directories(xnap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(xnap_test_helpers ocudu_support xnap_asn1 ocudulog)

set(SOURCES
        ngran_node_cfg_update_procedure_test.cpp
        retrieve_ue_context_asn1_helpers_test.cpp
        xn_setup_procedure_test.cpp
        xnap_asn1_converters_test.cpp
        xnap_handover_preparation_procedure_test.cpp
        xnap_retrieve_ue_context_procedure_test.cpp
        )

add_executable(xnap_test ${SOURCES})
target_include_directories(xnap_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(xnap_test xnap_test_helpers xnap_test_doubles ocudu_xnap ocudu_support rrc_ue_test_helpers ocudulog gtest gtest_main)
gtest_discover_tests(xnap_test PROPERTIES "LABELS;xnap")
