# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "pcap")

add_executable(pcap_mac_test mac_pcap_test.cpp)
target_link_libraries(pcap_mac_test ocudu_pcap ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pcap_mac_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_mac_test)

add_executable(pcap_rlc_test rlc_pcap_test.cpp)
target_link_libraries(pcap_rlc_test ocudu_pcap ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pcap_rlc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_rlc_test)

add_executable(pcap_ngap_test ngap_pcap_test.cpp)
target_link_libraries(pcap_ngap_test ocudu_pcap ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pcap_ngap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_ngap_test)
