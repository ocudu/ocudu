# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_library(ocudu_e1ap_cu_up
        e1ap_cu_up_factory.cpp
        e1ap_cu_up_impl.cpp
        e1ap_cu_up_connection_handler.cpp
        procedures/e1ap_cu_up_setup_procedure.cpp
        procedures/bearer_context_modification_procedure.cpp
        procedures/bearer_context_release_procedure.cpp
        procedures/e1ap_cu_up_release_procedure.cpp
        procedures/e1ap_cu_up_reset_procedure.cpp
        )
target_include_directories(ocudu_e1ap_cu_up PRIVATE ..)
target_link_libraries(ocudu_e1ap_cu_up e1ap_asn1 ocudu_e1ap_common)
