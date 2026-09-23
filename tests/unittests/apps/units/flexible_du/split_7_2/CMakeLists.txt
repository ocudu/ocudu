# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "ofh;apps")

add_executable(ru_ofh_config_cli11_schema_test ru_ofh_config_cli11_schema_test.cpp)
target_include_directories(ru_ofh_config_cli11_schema_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ru_ofh_config_cli11_schema_test
        ocudu_split_7_2_app_unit_helpers
        ocudu_app_services
        ocudu_yaml_util
        ocudu_gtest_main
        gtest)
add_test(ru_ofh_config_cli11_schema_test ru_ofh_config_cli11_schema_test)
