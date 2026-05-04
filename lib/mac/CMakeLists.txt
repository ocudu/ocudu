# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES config/mac_cell_group_config_factory.cpp config/mac_config_helpers.cpp)
add_library(mac_configuration_helpers STATIC ${SOURCES})
target_link_libraries(mac_configuration_helpers ocudu_support ocudu_ran)
set(SOURCES mac_factory.cpp
        mac_impl.cpp
        mac_ctrl/mac_controller.cpp
        mac_ctrl/ue_creation_procedure.cpp
        mac_ctrl/ue_reconfiguration_procedure.cpp
        mac_ctrl/mac_ue_removal_procedure.cpp
        mac_ctrl/mac_metrics_aggregator.cpp
        mac_dl/sib_pdu_assembler.cpp
        mac_dl/si_message_extension_handler_impl.cpp
        mac_dl/mac_dl_processor.cpp
        mac_dl/mac_subframe_time_mapper_impl.cpp
        mac_dl/mac_cell_processor.cpp
        mac_dl/rar_pdu_assembler.cpp
        mac_dl/dl_sch_pdu_assembler.cpp
        mac_dl/paging_pdu_assembler.cpp
        mac_dl/ssb_assembler.cpp
        mac_dl/mac_dl_ue_repository.cpp
        mac_dl/cell_dl_harq_buffer_pool.cpp
        mac_dl/mac_dl_metric_handler.cpp
        mac_ul/mac_ul_sch_pdu.cpp
        mac_ul/ul_bsr.cpp
        mac_ul/pdu_rx_handler.cpp
        mac_ul/mac_ul_processor.cpp
        mac_ul/mac_ul_ue_manager.cpp
        mac_sched/ocudu_scheduler_adapter.cpp
        mac_sched/mac_rach_handler.cpp
        mac_sched/uci_cell_decoder.cpp
        mac_sched/positioning_handler.cpp
)

add_library(ocudu_mac STATIC ${SOURCES})
target_link_libraries(ocudu_mac
        atomic
        ocudu_sched
        mac_configuration_helpers
        ocudu_ran
        ocudu_pcap
        rrc_nr_asn1
        ocudu_instrumentation)
