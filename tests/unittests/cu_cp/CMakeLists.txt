# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "cu_cp")

include_directories(../../..)

add_subdirectory(cell_meas_manager)
add_subdirectory(cu_up_processor)
add_subdirectory(du_processor)
add_subdirectory(ue_manager)
add_subdirectory(up_resource_manager)
add_subdirectory(mobility)
add_subdirectory(metrics_handler)

add_library(cu_cp_test_helpers
        test_doubles/mock_cu_up.cpp
        test_doubles/mock_amf.cpp
        test_doubles/mock_du.cpp
        test_doubles/mock_xnc_cu_cp.cpp
        cu_cp_test_environment.cpp
        cu_cp_test_messages.cpp
        du_processor_test_messages.cpp
        test_helpers.cpp)
target_include_directories(cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_test_helpers
        ocudu_cu_cp
        ocudu_support
        ocudulog
        e1ap_test_helpers
        e1ap_test_doubles
        f1ap_test_doubles
        ngap_test_helpers
        ngap_test_doubles
        xnap_test_doubles
        nrppa_test_doubles
        rrc_ue_test_helpers
        f1ap_asn1
        ngap_asn1
        nrppa_asn1
        xnap_asn1
        e1ap_asn1 ocudu_e2)


add_executable(cu_cp_test
               cu_cp_connectivity_test.cpp
               cu_cp_inactivity_notification_test.cpp
               cu_cp_initial_context_setup_test.cpp
               cu_cp_inter_cu_ng_handover_test.cpp
               cu_cp_inter_cu_xn_handover_test.cpp
               cu_cp_inter_du_handover_test.cpp
               cu_cp_intra_du_handover_test.cpp
               cu_cp_location_reporting_test.cpp
               cu_cp_mocn_test.cpp
               cu_cp_nrppa_test.cpp
               cu_cp_paging_test.cpp
               cu_cp_write_replace_warning_test.cpp
               cu_cp_pdu_session_resource_setup_test.cpp
               cu_cp_pdu_session_resource_release_test.cpp
               cu_cp_pdu_session_resource_modify_test.cpp
               cu_cp_reestablishment_test.cpp
               cu_cp_rrc_inactive_test.cpp
               cu_cp_setup_test.cpp
               cu_cp_ue_context_release_test.cpp
               cu_cp_xnap_test.cpp
               )
set_target_properties(cu_cp_test PROPERTIES UNITY_BUILD ON)
target_link_libraries(cu_cp_test
        cu_cp_test_helpers
        f1ap_cu_test_helpers
        ngap_test_helpers
        xnap_test_helpers
        rrc_ue_test_helpers
        ocudu_cu_cp
        ocudu_support
        ocudulog
        f1ap_asn1
        ngap_asn1
        rrc_test_doubles
        e1ap_test_doubles
        f1ap_test_doubles
        ngap_test_doubles
        xnap_test_doubles
        gtest
        ocudu_gtest_main)
target_include_directories(cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_test)

add_executable(cu_cp_config_test cu_cp_config_test.cpp)
target_link_libraries(cu_cp_config_test cu_cp_test_helpers ocudu_cu_cp GTest::gtest ocudu_gtest_main)
target_include_directories(cu_cp_config_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_config_test)
