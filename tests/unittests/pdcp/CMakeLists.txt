# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(pdcp_tx_test pdcp_tx_test.cpp)
target_link_libraries(pdcp_tx_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_test NO_PRETTY_VALUES PROPERTIES LABELS "tsan")

add_executable(pdcp_tx_empty_pool_test pdcp_tx_empty_pool_test.cpp)
target_link_libraries(pdcp_tx_empty_pool_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_empty_pool_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_empty_pool_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_test pdcp_rx_test.cpp)
target_link_libraries(pdcp_rx_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_test NO_PRETTY_VALUES PROPERTIES LABELS "tsan")

add_executable(pdcp_tx_metrics_test pdcp_tx_metrics_test.cpp)
target_link_libraries(pdcp_tx_metrics_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_metrics_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_metrics_test pdcp_rx_metrics_test.cpp)
target_link_libraries(pdcp_rx_metrics_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_metrics_test PROPERTIES LABELS "tsan")

add_executable(pdcp_tx_status_report_test pdcp_tx_status_report_test.cpp)
target_link_libraries(pdcp_tx_status_report_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_status_report_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_status_report_test pdcp_rx_status_report_test.cpp)
target_link_libraries(pdcp_rx_status_report_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_status_report_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_reestablish_test pdcp_rx_reestablish_test.cpp)
target_link_libraries(pdcp_rx_reestablish_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_reestablish_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_reestablish_test PROPERTIES LABELS "tsan")

add_executable(pdcp_tx_reestablish_test pdcp_tx_reestablish_test.cpp)
target_link_libraries(pdcp_tx_reestablish_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_reestablish_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_reestablish_test PROPERTIES LABELS "tsan")

add_executable(pdcp_tx_suspend_test pdcp_tx_suspend_test.cpp)
target_link_libraries(pdcp_tx_suspend_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_suspend_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_suspend_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_suspend_test pdcp_rx_suspend_test.cpp)
target_link_libraries(pdcp_rx_suspend_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_suspend_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_suspend_test PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_stop_test pdcp_rx_stop_test.cpp)
target_link_libraries(pdcp_rx_stop_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_rx_stop_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_stop_test)

add_executable(pdcp_tx_stop_test pdcp_tx_stop_test.cpp)
target_link_libraries(pdcp_tx_stop_test ocudu_pdcp ocudu_security ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(pdcp_tx_stop_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_stop_test)

add_executable(pdcp_gen_helper pdcp_gen_helper.cpp)
target_link_libraries(pdcp_gen_helper ocudu_pdcp ocudu_security ocudu_support ocudulog)
target_include_directories(pdcp_gen_helper PRIVATE ${CMAKE_SOURCE_DIR})
