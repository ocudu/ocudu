# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_library(test_rng test_rng.cpp)
target_link_libraries(test_rng PRIVATE ocudu_support)

add_library(ocudu_gtest_main ocudu_gtest_main.cpp)
target_link_libraries(ocudu_gtest_main PRIVATE test_rng ocudulog ocudu_support gtest)
