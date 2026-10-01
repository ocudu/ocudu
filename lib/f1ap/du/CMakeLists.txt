# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_f1ap_du_helpers f1ap_asn1_converters.cpp)
target_include_directories(ocudu_f1ap_du_helpers PRIVATE ..)
target_link_libraries(ocudu_f1ap_du_helpers f1ap_asn1)

add_subdirectory(procedures)
add_library(ocudu_f1ap_du
        f1ap_du_impl.cpp
        f1ap_du_factory.cpp
        f1ap_du_connection_handler.cpp
        f1ap_du_metrics_collector_impl.cpp
        ue_context/ue_bearer_manager.cpp
        ue_context/f1c_du_bearer_impl.cpp
        ue_context/f1ap_du_ue_config_update.cpp)
target_include_directories(ocudu_f1ap_du PRIVATE ..)
target_link_libraries(ocudu_f1ap_du f1ap_asn1 ocudu_f1ap_common ocudu_f1ap_du_procedures ocudu_f1u_du)
