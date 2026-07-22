# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(sched_test_doubles scheduler_result_finder.cpp
        cell_config_builder_profiles.cpp
        scheduler_config_helper.cpp
        scheduler_test_message_validators.cpp
        srs_resource_manager.cpp
)
set_target_properties(sched_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(sched_test_doubles ocudu_sched sched_rrm ocudulog)
target_include_directories(sched_test_doubles PRIVATE ../../../)
