# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ngap_asn1_packer.cpp
        ngap_asn1_utils.cpp
        log_helpers.cpp
        ngap_factory.cpp
        ngap_impl.cpp
        ngap_connection_handler.cpp
        ngap_validators/ngap_validators.cpp
        gateways/n2_connection_client_factory.cpp
        metrics/ngap_metrics_aggregator.cpp
        procedures/ng_setup_procedure.cpp
        procedures/ng_reset_procedure.cpp
        procedures/ngap_dl_nas_message_transfer_procedure.cpp
        procedures/ngap_dl_ran_status_transfer_procedure.cpp
        procedures/ngap_handover_preparation_procedure.cpp
        procedures/ngap_handover_resource_allocation_procedure.cpp
        procedures/ngap_initial_context_setup_procedure.cpp
        procedures/ngap_path_switch_procedure.cpp
        procedures/ngap_pdu_session_resource_modify_procedure.cpp
        procedures/ngap_pdu_session_resource_release_procedure.cpp
        procedures/ngap_pdu_session_resource_setup_procedure.cpp
        procedures/ngap_ue_context_modification_procedure.cpp
        procedures/ngap_ue_context_release_procedure.cpp
    )


add_library(ocudu_ngap STATIC ${SOURCES})

target_link_libraries(ocudu_ngap
	ocudu_cu_cp
	ngap_asn1
	ocudu_gateway
	ocudu_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
