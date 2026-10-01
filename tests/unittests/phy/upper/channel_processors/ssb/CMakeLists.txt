# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(ssb_processor_unittest ssb_processor_unittest.cpp)
target_link_libraries(ssb_processor_unittest ocudu_channel_processors ocudulog gtest
        gtest_main)
add_test(ssb_processor_unittest ssb_processor_unittest)
