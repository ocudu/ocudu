# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(common_scheduler_test
        ssb_scheduler_test.cpp
        sib1_scheduler_test.cpp
        si_scheduler_test.cpp
        prach_scheduler_test.cpp
        prach_scheduler_vectortest.cpp
        ra_scheduler_test.cpp
        ra_ue_repository_test.cpp
        paging_scheduler_test.cpp
        prs_scheduler_test.cpp
        sub_scheduler_test_environment.cpp)
target_link_libraries(common_scheduler_test
        ocudu_sched
        ocudulog
        sched_config
        sched_config_manager
        mac_configuration_helpers
        scheduler_test_suite
        sched_test_doubles
        gtest
        ocudu_gtest_main)
# The PRS tests are registered separately below, so that they can carry the MVP-FUNC-POS-16-2 label.
# --gtest_fail_if_no_test_selected makes a filter that no longer matches anything (e.g. after a test rename) fail
# instead of silently reporting a passing, empty run.
add_test(NAME common_scheduler_test
         COMMAND common_scheduler_test "--gtest_filter=-prs_scheduler_test.*")

add_test(NAME prs_scheduler_test
         COMMAND common_scheduler_test "--gtest_filter=prs_scheduler_test.*" --gtest_fail_if_no_test_selected)
set_tests_properties(prs_scheduler_test PROPERTIES LABELS "MVP-FUNC-POS-16-2")
