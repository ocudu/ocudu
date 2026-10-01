# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(cu_cp_metrics_handler_test metrics_handler_test.cpp)
target_include_directories(cu_cp_metrics_handler_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_metrics_handler_test ocudu_support ocudu_cu_cp ocudulog GTest::gtest GTest::gtest_main)
