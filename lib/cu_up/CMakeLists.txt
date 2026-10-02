# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        cu_up_executor_mapper.cpp
        cu_up_factory.cpp
        cu_up_impl.cpp
        cu_up_manager_impl.cpp
        ue_manager.cpp
        pdu_session_manager_impl.cpp
        ngu_session_manager_impl.cpp
        xnu_session_manager_impl.cpp
        routines/cu_up_setup_routine.cpp
        routines/cu_up_e1_connection_loss_routine.cpp
        routines/cu_up_bearer_context_modification_routine.cpp
        routines/cu_up_test_mode_routines.cpp
)

add_library(ocudu_cu_up STATIC ${SOURCES})
target_link_libraries(ocudu_cu_up
        ocudu_e1ap_common
        ocudu_e1ap_cu_up
        ocudu_gtpu
        ocudu_pdcp
        ocudu_f1u_cu_up
        ocudu_gateway
        e1ap_asn1
        ocudu_rohc
        ocudu_sdap
        ocudu_security
        ocudu_support
        ocudulog)

add_library(ocudu_o_cu_up STATIC
        o_cu_up_factory.cpp
        o_cu_up_impl.cpp)
target_link_libraries(ocudu_o_cu_up PRIVATE ocudu_cu_up ocudu_e2)
