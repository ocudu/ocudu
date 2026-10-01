# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_executable(f1_gateway_helpers_test f1_gateway_helpers_test.cpp)
target_include_directories(f1_gateway_helpers_test PRIVATE ${CMAKE_SOURCE_DIR})
# The test includes f1_gateway_helpers.cpp directly (see file header comment) to access its file-local static
# configuration helpers, so it needs the same transitive dependencies as ocudu_f1_app_helpers itself.
target_link_libraries(f1_gateway_helpers_test PRIVATE
        ocudu_f1c_gateway
        ocudu_gtpu
        ocudu_f1u_cu_up_split_connector
        ocudu_f1u_du_split_connector
        GTest::gtest
        GTest::gtest_main)
gtest_discover_tests(f1_gateway_helpers_test)

add_subdirectory(config)
