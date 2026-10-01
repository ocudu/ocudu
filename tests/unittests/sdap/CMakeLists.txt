# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(sdap_test sdap_test.cpp)
target_link_libraries(sdap_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(sdap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_test)

add_executable(sdap_tx_test sdap_tx_test.cpp)
target_link_libraries(sdap_tx_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(sdap_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_tx_test)

add_executable(sdap_rx_test sdap_rx_test.cpp)
target_link_libraries(sdap_rx_test ocudu_support ocudulog GTest::gtest GTest::gtest_main)
target_include_directories(sdap_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_rx_test)
