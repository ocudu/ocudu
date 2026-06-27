# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


include_directories(../../..)

set_directory_properties(PROPERTIES LABELS "f1ap;f1ap_du")

add_library(f1ap_du_test_helpers f1ap_du_test_helpers.cpp
        f1ap_du_reset_test.cpp
)
target_link_libraries(f1ap_du_test_helpers f1ap_test_doubles
        ocudu_f1ap_du ocudu_support du_manager_converters sched_config)

set(SOURCES f1ap_du_setup_procedure_test.cpp
            f1ap_du_dl_rrc_message_transfer_test.cpp
            f1ap_du_ue_config_test.cpp
            f1ap_du_ue_context_setup_procedure_test.cpp
            f1ap_du_ue_context_release_test.cpp
            f1ap_du_ue_context_modification_test.cpp
            f1ap_du_reset_test.cpp
            f1ap_du_ul_rrc_message_transfer_test.cpp
            f1ap_du_gnbdu_config_update_test.cpp
            f1ap_du_gnbcu_config_update_test.cpp
            f1ap_du_positioning_exchange_procedure_test.cpp
            f1ap_du_trp_information_exchange_procedure_test.cpp
            f1ap_du_positioning_measurement_procedure_test.cpp
            f1ap_du_paging_test.cpp
            f1ap_du_ref_time_info_report_test.cpp
)

add_executable(f1ap_du_test ${SOURCES})
target_include_directories(f1ap_du_test PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(f1ap_du_test PROPERTIES UNITY_BUILD ON)
target_link_libraries(f1ap_du_test
        ocudu_f1ap_du
        ocudu_support
        du_manager_converters
        ocudu_ran
        sched_config
        f1ap_du_test_helpers
        gtest
        ocudu_gtest_main)
gtest_discover_tests(f1ap_du_test PROPERTIES "LABELS;f1ap_du")
