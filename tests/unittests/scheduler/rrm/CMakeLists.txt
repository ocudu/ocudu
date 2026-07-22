# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(pucch_resource_manager_test pucch_resource_manager_test.cpp)
target_link_libraries(pucch_resource_manager_test
        ocudu_sched
        sched_config
        scheduler_test_suite
        sched_test_doubles
        sched_rrm
        gtest
        gtest_main)
gtest_discover_tests(pucch_resource_manager_test)

add_executable(srs_resource_generator_test srs_resource_generator_test.cpp)
target_include_directories(srs_resource_generator_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srs_resource_generator_test sched_rrm sched_config du_manager_test_helpers gtest ocudu_gtest_main)
gtest_discover_tests(srs_resource_generator_test)

add_executable(srs_resource_manager_periodic_test srs_resource_manager_periodic_test.cpp)
target_include_directories(srs_resource_manager_periodic_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srs_resource_manager_periodic_test sched_rrm sched_config du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(srs_resource_manager_periodic_test)

add_executable(srs_resource_manager_aperiodic_test srs_resource_manager_aperiodic_test.cpp)
target_include_directories(srs_resource_manager_aperiodic_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srs_resource_manager_aperiodic_test sched_rrm sched_config du_manager_test_helpers sched_test_doubles gtest ocudu_gtest_main)
gtest_discover_tests(srs_resource_manager_aperiodic_test)
