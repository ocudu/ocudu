# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pucch_processor_format1_unittest pucch_processor_format1_unittest.cpp)
target_link_libraries(pucch_processor_format1_unittest
        ocudu_channel_equalizer
        ocudu_channel_processors
        ocudu_dft
        ocudu_transform_precoding
        ocudulog
        GTest::gtest
        GTest::gtest_main)
add_test(pucch_processor_format1_unittest pucch_processor_format1_unittest)

add_executable(pucch_processor_validators_test
        pucch_processor_validator_format0_test.cpp
        pucch_processor_validator_format1_test.cpp
        pucch_processor_validator_format2_test.cpp
        pucch_processor_validator_format3_test.cpp
        pucch_processor_validator_format4_test.cpp)
target_link_libraries(pucch_processor_validators_test
        ocudu_channel_equalizer
        ocudu_channel_processors
        ocudu_dft
        ocudu_phy_support
        ocudu_transform_precoding
        ocudu_upper_phy_support
        ocudulog
        GTest::gtest
        GTest::gtest_main)
add_test(pucch_processor_validators_test pucch_processor_validators_test)
