# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(f1u_du_bearer_test f1u_du_bearer_test.cpp)
target_link_libraries(f1u_du_bearer_test ocudu_f1u_du ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(f1u_du_bearer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_du_bearer_test)
