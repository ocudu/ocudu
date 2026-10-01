# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(gateways)

set(SOURCES
        log_helpers.cpp
        procedures/ngran_node_cfg_update_procedure.cpp
        procedures/xnap_source_handover_preparation_procedure.cpp
        procedures/xnap_old_node_retrieve_ue_context_procedure.cpp
        procedures/xnap_target_handover_preparation_procedure.cpp
        procedures/xnap_new_node_retrieve_ue_context_procedure.cpp
        procedures/xn_setup_procedure.cpp
        procedures/xnap_sn_status_transfer_procedure.cpp
        xnap_asn1_utils.cpp
        xnap_factory.cpp
        xnap_impl.cpp
    )

add_library(ocudu_xnap STATIC ${SOURCES})
target_link_libraries(ocudu_xnap
	ocudu_cu_cp
	xnap_asn1
	ocudu_support)
