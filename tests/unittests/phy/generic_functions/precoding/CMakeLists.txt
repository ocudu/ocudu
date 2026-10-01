# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(channel_precoder_test channel_precoder_test.cpp)
target_include_directories(channel_precoder_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests)

target_link_libraries(
        channel_precoder_test
        ocudu_channel_precoder
        ocudu_ran
        ocudulog
        ocuduvec
        GTest::gtest
        GTest::gtest_main)

add_test(channel_precoder_test channel_precoder_test)
