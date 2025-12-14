# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(sched_config STATIC
        builder/pucch_config.cpp
        builder/csi_meas_config.cpp
        ran_cell_config_helper.cpp
        serving_cell_config_factory.cpp
        sched_cell_config_helpers.cpp
        scheduler_cell_config_validator.cpp
        scheduler_ue_config_validator.cpp
        serving_cell_config_validator.cpp
        csi_helper.cpp
        pucch_default_resource.cpp
        pucch_guardbands.cpp
        pucch_resource_generator.cpp
        time_domain_resource_helper.cpp
        rlm_helper.cpp
        bwp_td_res_info.cpp)
target_link_libraries(sched_config ocudu_ran du_resource_manager)

add_library(sched_config_manager OBJECT
		cell_bwp_res_config.cpp
        cell_configuration.cpp
		pdcch_config_pool.cpp
        ue_configuration.cpp
        sched_config_manager.cpp
        du_cell_group_config_pool.cpp
)
target_link_libraries(sched_config_manager ocudu_ran)
