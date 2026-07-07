# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(resource_grid_pool_test resource_grid_pool_test.cpp)
target_link_libraries(resource_grid_pool_test ocuduvec ocudu_phy_support ocudulog)
add_test(resource_grid_pool_test resource_grid_pool_test)

add_executable(resource_grid_test resource_grid_test.cpp)
target_link_libraries(resource_grid_test
        ocuduvec
        ocudu_phy_support
        ocudulog
        GTest::gtest
        GTest::gtest_main)
add_test(resource_grid_test resource_grid_test)

add_executable(interpolator_test interpolator_test.cpp)
target_link_libraries(interpolator_test ocudu_phy_support ocudulog GTest::gtest GTest::gtest_main)
add_test(interpolator_test interpolator_test)

add_executable(resource_grid_mapper_test resource_grid_mapper_test.cpp)
target_link_libraries(resource_grid_mapper_test
        ocuduvec
        ocudu_phy_support
        ocudu_upper_phy_support
        ocudu_channel_precoder
        ocudu_ran
        ocudulog
        GTest::gtest
        GTest::gtest_main)
gtest_discover_tests(resource_grid_mapper_test)

add_executable(precoding_configuration_test precoding_configuration_test.cpp)
target_link_libraries(precoding_configuration_test
        ocudulog
        GTest::gtest
        GTest::gtest_main)
gtest_discover_tests(precoding_configuration_test)

add_executable(re_pattern_test re_pattern_test.cpp)
target_link_libraries(re_pattern_test ocudu_phy_support ocudulog)
add_test(re_pattern_test re_pattern_test)
