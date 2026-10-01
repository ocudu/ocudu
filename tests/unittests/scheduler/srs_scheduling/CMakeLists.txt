# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(srs_sched_test
        srs_scheduler_test.cpp
        srs_allocator_test.cpp)
target_link_libraries(srs_sched_test
        ocudu_sched
        scheduler_test_suite
        ocudulog
        sched_config
        sched_test_doubles
        gtest
        ocudu_gtest_main)
gtest_discover_tests(srs_sched_test)
