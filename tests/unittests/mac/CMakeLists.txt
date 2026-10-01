# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "mac")

include_directories(../../..)

add_executable(mac_test
        mac_test_helpers.cpp
        mac_ctrl_test_dummies.cpp
        mac_ue_create_test.cpp
        mac_ctrl_test.cpp
        mac_ul_pdu_test.cpp
        mac_ul_processor_test.cpp
        mac_rlf_detector_test.cpp
        uci_cell_decoder_test.cpp
        mac_cell_processor_test.cpp
        mac_cell_pcap_writer_test.cpp
        cell_dl_harq_buffer_pool_test.cpp
        dl_sch_pdu_assembler_test.cpp
        mac_rar_pdu_assembler_test.cpp
        sib_pdu_assembler_test.cpp
        si_message_controller_test.cpp
        ssb_assembler_test.cpp
        rnti_manager_test.cpp
        mac_rach_handler_test.cpp
        mac_dl_cfg_test.cpp
        mac_metrics_test.cpp
        mac_config_helpers_test.cpp
        subframe_time_mapper_test.cpp
        mac_positioning_handler_test.cpp
)
target_link_libraries(mac_test
        ocudu_pcap
        ocudu_support
        ocudulog
        rrc_nr_asn1
        ocudu_mac
        sched_config
        sched_test_doubles
        GTest::gtest
        ocudu_gtest_main)
gtest_discover_tests(mac_test)
