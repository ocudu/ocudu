# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "apps")

add_executable(remote_command_dispatcher_test remote_command_dispatcher_test.cpp)
target_include_directories(remote_command_dispatcher_test PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(remote_command_dispatcher_test
        ocudu_remote_control_app_service
        ocudu_versioning
        ocudu_support
        GTest::gtest
        ocudu_gtest_main)
gtest_discover_tests(remote_command_dispatcher_test)
