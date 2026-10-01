# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(config_yaml_schema_test config_yaml_schema_test.cpp)
target_include_directories(config_yaml_schema_test PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(config_yaml_schema_test
        ocudu_config_app_helpers
        ocudu_support
        ocudu_yaml_util
        GTest::gtest
        ocudu_gtest_main
        ocudu::yaml-cpp)
add_test(config_yaml_schema_test config_yaml_schema_test)
