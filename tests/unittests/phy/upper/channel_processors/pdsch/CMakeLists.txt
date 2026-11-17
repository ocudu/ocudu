# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest ocudu_channel_processors ocudulog GTest::gtest GTest::gtest_main)
add_test(pdsch_processor_unittest pdsch_processor_unittest)

add_executable(pdsch_processor_validator_test pdsch_processor_validator_test.cpp)
target_link_libraries(pdsch_processor_validator_test
        ocudu_channel_equalizer
        ocudu_channel_precoder
        ocudu_channel_processors
        ocudu_phy_support
        ocudulog
        ocudu_upper_phy_support
        GTest::gtest
        GTest::gtest_main)
add_test(pdsch_processor_validator_test pdsch_processor_validator_test)

add_executable(pdsch_codeword_interleaver_test pdsch_codeword_interleaver_test.cpp)
target_link_libraries(pdsch_codeword_interleaver_test
        ocudu_pdsch_processor
        ocudu_phy_support
        ocudulog
        GTest::gtest
        GTest::gtest_main)
target_include_directories(pdsch_codeword_interleaver_test PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/utils
        ${CMAKE_SOURCE_DIR}/lib/phy/upper/channel_processors/pdsch)
add_test(pdsch_codeword_interleaver_test pdsch_codeword_interleaver_test)
