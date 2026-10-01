# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_library(ocudu_e1ap_cu_cp
        e1ap_cu_cp_factory.cpp
        e1ap_cu_cp_impl.cpp
        procedures/bearer_context_setup_procedure.cpp
        procedures/bearer_context_release_procedure.cpp
        procedures/bearer_context_modification_procedure.cpp
        procedures/cu_cp_e1_reset_procedure.cpp
        procedures/e1_release_procedure.cpp
        procedures/e1ap_stop_procedure.cpp
        ue_context/e1ap_cu_cp_ue_context.cpp)
target_include_directories(ocudu_e1ap_cu_cp PRIVATE ..)
target_link_libraries(ocudu_e1ap_cu_cp e1ap_asn1 ocudu_e1ap_common ocudu_cu_cp)
