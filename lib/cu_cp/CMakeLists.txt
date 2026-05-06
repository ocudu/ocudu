# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(cell_meas_manager)
add_subdirectory(mobility_manager)
add_subdirectory(ue_location_manager)
add_subdirectory(up_resource_manager)
add_subdirectory(ue_security_manager)

set(SOURCES
        cu_cp_factory.cpp
        cu_cp_impl.cpp
        cu_configurator_impl.cpp
        cu_cp_executor_mapper.cpp
        cu_cp_controller/cu_cp_controller.cpp
        cu_cp_controller/amf_connection_manager.cpp
        cu_cp_controller/du_connection_manager.cpp
        cu_cp_controller/cu_up_connection_manager.cpp
        cu_cp_controller/xnc_connection_manager.cpp
        cu_up_processor/cu_up_processor_impl.cpp
        cu_up_processor/cu_up_processor_factory.cpp
        cu_up_processor/cu_up_processor_repository.cpp
        du_processor/du_configuration_manager.cpp
        du_processor/du_processor_impl.cpp
        du_processor/du_processor_factory.cpp
        du_processor/du_processor_repository.cpp
        metrics_handler/metrics_handler_impl.cpp
        ngap_repository.cpp
        paging/paging_message_handler.cpp
        routines/amf_connection_loss_routine.cpp
        routines/amf_connection_setup_routine.cpp
        routines/amf_connection_removal_routine.cpp
        routines/amf_reconnection_routine.cpp
        routines/cell_activation_routine.cpp
        routines/cell_deactivation_routine.cpp
        routines/initial_context_setup_routine.cpp
        routines/pdu_session_routine_helpers.cpp
        routines/pdu_session_resource_setup_routine.cpp
        routines/pdu_session_resource_release_routine.cpp
        routines/pdu_session_resource_modification_routine.cpp
        routines/reestablishment_context_modification_routine.cpp
        routines/ue_context_release_routine.cpp
        routines/ue_removal_routine.cpp
        routines/ue_resume_routine.cpp
        routines/ue_suspend_routine.cpp
        routines/ue_transaction_info_release_routine.cpp
        routines/ue_amf_context_release_request_routine.cpp
        routines/mobility/mobility_helpers.cpp
        routines/mobility/intra_cu_handover_routine.cpp
        routines/mobility/intra_cu_handover_target_routine.cpp
        routines/mobility/inter_cu_handover_source_routine.cpp
        routines/mobility/inter_cu_handover_target_routine.cpp
        routines/mobility/inter_cu_handover_execution_target_routine.cpp
        routines/mobility/handover_reconfiguration_routine.cpp
        routines/mobility/conditional_handover_cancellation_routine.cpp
        routines/mobility/conditional_handover_coordinator_routine.cpp
        routines/mobility/conditional_handover_reconfiguration_routine.cpp
        routines/mobility/conditional_handover_source_routine.cpp
        routines/mobility/conditional_handover_target_routine.cpp
        routines/mobility/inter_cu_conditional_handover_source_completion_routine.cpp
        routines/mobility/inter_cu_conditional_handover_target_execution_routine.cpp
        routines/positioning/trp_information_exchange_routine.cpp
        task_schedulers/ngap_task_scheduler.cpp
        task_schedulers/xnap_task_scheduler.cpp
        ue_manager/cu_cp_ue_impl.cpp
        ue_manager/ue_manager_impl.cpp
        ue_manager/ue_task_scheduler_impl.cpp
        xnap_repository.cpp
)

add_library(ocudu_cu_cp STATIC ${SOURCES})

target_link_libraries(ocudu_cu_cp
        ocudu_cu_cp_cell_meas_manager
        ocudu_cu_cp_mobility_manager
        ocudu_ue_location_manager
        ocudu_ue_security_manager
        ocudu_e1ap_cu_cp
        ocudu_f1ap_cu
        ocudu_ngap
        ocudu_xnap
        ocudu_nrppa
        ocudu_rrc
        ocudu_ran
        ocudulog
        ocudu_support
        ocudu_security
        ocudu_e2
        ${CMAKE_DL_LIBS}
)

add_library(ocudu_o_cu_cp o_cu_cp_factory.cpp o_cu_cp_impl.cpp)
target_link_libraries(ocudu_o_cu_cp PRIVATE ocudu_cu_cp)
