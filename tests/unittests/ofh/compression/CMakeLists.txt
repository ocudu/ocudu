# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "compression")

add_executable(ofh_iq_compression_bfp_test ofh_iq_compression_bfp_test.cpp)
target_link_libraries(ofh_iq_compression_bfp_test ocudu_ofh_compression ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_iq_compression_bfp_test)
