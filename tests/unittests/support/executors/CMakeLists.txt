# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "support")

add_executable(task_worker_test
        sync_task_executor_test.cpp
        task_worker_test.cpp
        strand_executor_test.cpp
        task_execution_manager_test.cpp
        metrics_executor_test.cpp
        fork_limit_executor_test.cpp)
target_link_libraries(task_worker_test ocudulog ocudu_support GTest::gtest ocudu_gtest_main)
gtest_discover_tests(task_worker_test PROPERTIES LABELS "tsan")

add_executable(execute_until_success_test execute_until_success_test.cpp)
target_link_libraries(execute_until_success_test ocudulog ocudu_support GTest::gtest ocudu_gtest_main)
gtest_discover_tests(execute_until_success_test)
