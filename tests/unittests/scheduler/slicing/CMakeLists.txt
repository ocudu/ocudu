# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(slice_scheduler_test slice_scheduler_test.cpp slice_ue_repository_test.cpp)
target_link_libraries(slice_scheduler_test
        ocudu_sched
        scheduler_test_utils
        scheduler_test_suite
        mac_configuration_helpers
        sched_test_doubles
        gtest
        gtest_main
)
gtest_discover_tests(slice_scheduler_test)
