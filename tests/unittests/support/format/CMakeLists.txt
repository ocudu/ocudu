# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "format")

add_executable(format_optional_test format_optional_test.cpp)
target_link_libraries(format_optional_test ocudulog ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(format_optional_test)
