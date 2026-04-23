# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "du_high;apps")

add_subdirectory(metrics)

add_executable(du_high_remote_commands_test du_high_remote_commands_test.cpp)
target_include_directories(du_high_remote_commands_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_high_remote_commands_test
        ocudu_du_high_unit_helpers
        ocudu_app_services
        ocudu_gtest_main
        gtest)
add_test(du_high_remote_commands_test du_high_remote_commands_test)
