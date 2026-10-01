# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "rlc")

add_executable(rlc_rx_am_test rlc_rx_am_test.cpp)
target_link_libraries(rlc_rx_am_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_rx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_am_test)

add_executable(rlc_tx_am_test rlc_tx_am_test.cpp)
target_link_libraries(rlc_tx_am_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_tx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_am_test)

add_executable(rlc_rx_tm_test rlc_rx_tm_test.cpp)
target_link_libraries(rlc_rx_tm_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_rx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_tm_test)

add_executable(rlc_tx_tm_test rlc_tx_tm_test.cpp)
target_link_libraries(rlc_tx_tm_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_tx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_tm_test)

add_executable(rlc_um_test rlc_um_test.cpp)
target_link_libraries(rlc_um_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_um_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_um_test)

add_executable(rlc_um_pdu_test rlc_um_pdu_test.cpp)
target_link_libraries(rlc_um_pdu_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_um_pdu_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_um_pdu_test)

add_executable(rlc_am_pdu_test rlc_am_pdu_test.cpp)
target_link_libraries(rlc_am_pdu_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_am_pdu_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_am_pdu_test)

add_executable(rlc_pdu_recycler_test rlc_pdu_recycler_test.cpp)
target_link_libraries(rlc_pdu_recycler_test ocudu_instrumentation ocudu_support ocudulog GTest::gtest GTest::gtest_main pdcp_test_doubles)
target_include_directories(rlc_pdu_recycler_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_pdu_recycler_test)

add_executable(rlc_sdu_queue_lockfree_test rlc_sdu_queue_lockfree_test.cpp)
target_link_libraries(rlc_sdu_queue_lockfree_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_sdu_queue_lockfree_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_sdu_queue_lockfree_test)

add_executable(rlc_retx_queue_test rlc_retx_queue_test.cpp)
target_link_libraries(rlc_retx_queue_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_retx_queue_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_retx_queue_test)

add_executable(rlc_bearer_metrics_collector_test rlc_bearer_metrics_collector_test.cpp)
target_link_libraries(rlc_bearer_metrics_collector_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_bearer_metrics_collector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_bearer_metrics_collector_test)

add_executable(rlc_tx_metrics_test rlc_tx_metrics_test.cpp)
target_link_libraries(rlc_tx_metrics_test ocudu_rlc ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(rlc_tx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_metrics_test)
