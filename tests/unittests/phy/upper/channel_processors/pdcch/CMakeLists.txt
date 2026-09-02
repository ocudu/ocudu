# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pdcch_processor_unittest pdcch_processor_unittest.cpp)
target_link_libraries(pdcch_processor_unittest ocudu_channel_processors ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(pdcch_processor_unittest)
target_include_directories(pdcch_processor_unittest PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/upper/signal_processors/pdcch)
