# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



set(SOURCES
        asn1_csi_meas_config_helpers.cpp
        asn1_ntn_config_helpers.cpp
        asn1_rrc_config_helpers.cpp
        asn1_sys_info_packer.cpp
        asn1_sys_info_packer_helpers.cpp
        f1ap_configuration_helpers.cpp
        rlc_config_helpers.cpp
        scheduler_configuration_helpers.cpp)

add_library(du_manager_converters STATIC ${SOURCES})
target_link_libraries(du_manager_converters rrc_nr_asn1 lpp_asn1 ocudu_support ocudu_ran ocudu_sched ocudu_cbs_encoder)
