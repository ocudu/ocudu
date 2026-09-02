# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(polar_chain_test polar_chain_test.cpp)
target_link_libraries(polar_chain_test ocudu_channel_coding ocuduvec ocudulog GTest::gtest)
add_test(polar_chain_test_bch polar_chain_test -n9 -k56 -e864 -i0 -s101 -o1)
add_test(polar_chain_test_dci polar_chain_test -n9 -k40 -e100 -i0 -s101 -o1)
add_test(polar_chain_test_uci polar_chain_test -n10 -k20 -e256 -i1 -s101 -o1)
add_test(polar_chain_test_uci2 polar_chain_test -n10 -k18 -e45 -i1 -s101 -o1)
add_test(polar_chain_test_uci3 polar_chain_test -n10 -k18 -e38 -i1 -s101 -o1)

add_executable(polar_interleaver_test polar_interleaver_test.cpp)
target_link_libraries(polar_interleaver_test ocudu_channel_coding ocuduvec ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(polar_interleaver_test)
