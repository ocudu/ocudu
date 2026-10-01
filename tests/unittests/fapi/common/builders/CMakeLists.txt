# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(error_indication_builder_test error_indication_test.cpp)
target_link_libraries(error_indication_builder_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(error_indication_builder_test)
