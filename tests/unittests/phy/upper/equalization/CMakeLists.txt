# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(channel_equalizer_support_test channel_equalizer_support.cpp)
target_link_libraries(channel_equalizer_support_test ocudu_channel_equalizer GTest::gtest GTest::gtest_main)
add_test(channel_equalizer_support_test channel_equalizer_support_test)
