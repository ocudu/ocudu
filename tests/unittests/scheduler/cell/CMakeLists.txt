# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(scheduler_cell_resource_test
        sched_prb_test.cpp
        cell_resource_grid_test.cpp
        cell_harq_manager_test.cpp
        sched_result_grid_test.cpp)
target_link_libraries(scheduler_cell_resource_test
        ocudu_sched
        ocudulog
        mac_configuration_helpers
        sched_config
        scheduler_test_utils
        sched_test_doubles
        gtest
        ocudu_gtest_main)
add_test(scheduler_cell_resource_test scheduler_cell_resource_test)

