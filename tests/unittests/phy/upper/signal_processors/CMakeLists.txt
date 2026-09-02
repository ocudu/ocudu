# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pss_processor_test ssb/pss_processor_test.cpp)
target_link_libraries(pss_processor_test ocudu_ssb_generators ocuduvec ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(pss_processor_test)

add_executable(sss_processor_test ssb/sss_processor_test.cpp)
target_link_libraries(sss_processor_test ocudu_ssb_generators ocuduvec ocudulog GTest::gtest ocudu_gtest_main)
gtest_discover_tests(sss_processor_test)

add_executable(prs_generator_validator_test prs/prs_generator_validator_test.cpp)
target_link_libraries(prs_generator_validator_test ocudu_prs_generator ocudu_ran ocuduvec ocudu_sequence_generators ocudu_channel_precoder ocudulog GTest::gtest GTest::gtest_main)
add_test(prs_generator_validator_test prs_generator_validator_test)
