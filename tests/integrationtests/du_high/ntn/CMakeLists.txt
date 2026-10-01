# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "ntn")

add_executable(ntn_config_update_remote_command_test ntn_config_update_remote_command_test.cpp)
target_include_directories(ntn_config_update_remote_command_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ntn_config_update_remote_command_test ocudu_flexible_o_du_helpers ocudu_support ocudu_ran ocudulog gtest gtest_main)
gtest_discover_tests(ntn_config_update_remote_command_test)
