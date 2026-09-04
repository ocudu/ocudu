# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "gtpu")

include_directories(../../..)

add_executable(gtpu_test gtpu_test.cpp)
target_link_libraries(gtpu_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_test)

add_executable(gtpu_demux_test gtpu_demux_test.cpp)
target_link_libraries(gtpu_demux_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_demux_test)

add_executable(gtpu_echo_test gtpu_echo_test.cpp)
target_link_libraries(gtpu_echo_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_echo_test)

add_executable(gtpu_tunnel_psup_test gtpu_tunnel_psup_test.cpp)
target_link_libraries(gtpu_tunnel_psup_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_tunnel_psup_test)

add_executable(gtpu_tunnel_psup_rx_test gtpu_tunnel_psup_rx_test.cpp)
target_link_libraries(gtpu_tunnel_psup_rx_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_tunnel_psup_rx_test)

add_executable(gtpu_tunnel_psup_tx_test gtpu_tunnel_psup_tx_test.cpp)
target_link_libraries(gtpu_tunnel_psup_tx_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_tunnel_psup_tx_test)

add_executable(gtpu_tunnel_nru_test gtpu_tunnel_nru_test.cpp)
target_link_libraries(gtpu_tunnel_nru_test ocudu_gtpu ocudu_pcap ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_tunnel_nru_test)

add_executable(gtpu_tunnel_nru_rx_test gtpu_tunnel_nru_rx_test.cpp)
target_link_libraries(gtpu_tunnel_nru_rx_test ocudu_gtpu ocudu_pcap ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_tunnel_nru_rx_test)

add_executable(gtpu_alloc_test gtpu_test_allocator.cpp)
target_link_libraries(gtpu_alloc_test ocudu_gtpu ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(gtpu_alloc_test)
