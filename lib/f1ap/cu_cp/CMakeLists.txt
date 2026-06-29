# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_library(ocudu_f1ap_cu
        f1ap_cu_impl.cpp
        f1ap_cu_factory.cpp
        procedures/f1_setup_procedure.cpp
        procedures/f1_removal_procedure.cpp
        procedures/f1ap_positioning_information_exchange_procedure.cpp
        procedures/f1ap_positioning_activation_procedure.cpp
        procedures/f1ap_positioning_measurement_procedure.cpp
        procedures/f1ap_stop_procedure.cpp
        procedures/f1ap_trp_information_exchange_procedure.cpp
        procedures/gnb_cu_configuration_update_procedure.cpp
        procedures/write_replace_warning_procedure.cpp
        procedures/ue_context_setup_procedure.cpp
        procedures/ue_context_modification_procedure.cpp
        procedures/ue_context_release_procedure.cpp
        ue_context/f1ap_cu_ue_context.cpp
        ue_context/ue_ul_bearer_manager.cpp)

target_include_directories(ocudu_f1ap_cu PRIVATE ..)
target_link_libraries(ocudu_f1ap_cu ocudu_support ocudu_ran ocudu_f1ap_common f1ap_asn1)
