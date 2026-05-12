# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(du_resource_manager
        du_bearer_resource_manager.cpp
        du_pdsch_resource_manager.cpp
        du_pucch_resource_manager.cpp
        du_pusch_resource_manager.cpp
        du_ran_resource_manager_impl.cpp
        du_srs_periodic_res_mng.cpp
        srs_resource_generator.cpp
        ue_capability_manager.cpp
        du_drx_resource_manager.cpp
        du_meas_config_manager.cpp
        ra_resource_manager.cpp
        du_srs_aperiodic_res_mng.cpp
        du_srs_manager_helpers.cpp
)
target_link_libraries(du_resource_manager
        du_manager_converters
        ocudu_du_config_validators
        mac_configuration_helpers
        sched_config
        sched_config_manager)
