# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ngap_test_helpers ngap_test_helpers.cpp ngap_test_messages.cpp)
target_include_directories(ngap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_test_helpers ocudu_support ngap_asn1 ocudulog)

set(SOURCES
        ng_setup_procedure_test.cpp
        ngap_asn1_packer_test.cpp
        ngap_error_indication_test.cpp
        ngap_handover_preparation_procedure_test.cpp
        ngap_handover_test.cpp
        ngap_location_reporting_test.cpp
        ngap_nas_message_test.cpp
        ngap_paging_test.cpp
        ngap_path_switch_procedure_test.cpp
        ngap_pdu_session_resource_setup_procedure_test.cpp
        ngap_pdu_session_resource_modify_procedure_test.cpp
        ngap_pdu_session_resource_release_procedure_test.cpp
        ngap_ue_context_management_procedure_test.cpp
        ngap_ue_context_test.cpp
        ngap_validators_test.cpp
        ngap_write_replace_warning_test.cpp
        )

add_executable(ngap_test ${SOURCES})
target_include_directories(ngap_test PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(ngap_test PROPERTIES UNITY_BUILD ON)
target_link_libraries(ngap_test ngap_test_helpers ocudu_ngap ocudu_support ngap_asn1 ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(ngap_test PROPERTIES "LABELS;ngap")
