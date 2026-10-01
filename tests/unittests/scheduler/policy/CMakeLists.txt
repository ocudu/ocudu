# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(scheduler_policy_test scheduler_policy_test.cpp)
target_link_libraries(scheduler_policy_test
        ocudu_sched
        scheduler_test_suite
        sched_test_doubles
        ocudulog
        gtest
        ocudu_gtest_main)
gtest_discover_tests(scheduler_policy_test)
