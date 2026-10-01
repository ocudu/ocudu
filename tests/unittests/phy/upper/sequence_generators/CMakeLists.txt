# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pseudo_random_test pseudo_random_generator_test.cpp)
target_link_libraries(pseudo_random_test ocudu_sequence_generators ocuduvec ocudulog GTest::gtest GTest::gtest_main)
add_test(pseudo_random_test pseudo_random_test)
