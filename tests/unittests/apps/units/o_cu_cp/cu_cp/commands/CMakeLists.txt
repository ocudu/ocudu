# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "cu_cp;apps")

add_executable(cu_cp_remote_commands_test cu_cp_remote_commands_test.cpp)
target_include_directories(cu_cp_remote_commands_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_remote_commands_test
        ocudu_cu_cp_app_unit
        ocudu_app_services
        ocudu_yaml_util
        ocudu_gtest_main
        gtest)
add_test(cu_cp_remote_commands_test cu_cp_remote_commands_test)
