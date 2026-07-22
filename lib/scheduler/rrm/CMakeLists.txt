# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(sched_rrm STATIC
        pucch_resource_manager.cpp
        srs_resource_generator.cpp
        srs_resource_manager_helpers.cpp
        srs_resource_manager_periodic.cpp
        srs_resource_manager_aperiodic.cpp
        srs_resource_manager_factory.cpp
)
target_link_libraries(sched_rrm ocudu_ran sched_config)
