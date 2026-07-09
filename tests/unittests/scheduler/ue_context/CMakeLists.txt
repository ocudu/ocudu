# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(sched_ue_context_test
        ue_drx_controller_test.cpp
        ue_repository_test.cpp
        logical_channel_system_test.cpp
        ta_management_system_test.cpp
        pucch_power_control_test.cpp
        pusch_power_control_test.cpp)
target_link_libraries(sched_ue_context_test
        sched_cell
        sched_support
        sched_ue_context
        sched_test_doubles
        sched_rrm
        scheduler_test_utils
        ocudu_support
        ocudulog
        gtest
        ocudu_gtest_main)
add_test(sched_ue_context_test sched_ue_context_test)
