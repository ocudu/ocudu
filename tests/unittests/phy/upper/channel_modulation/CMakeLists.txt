# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(evm_calculator_test evm_calculator_test.cpp)
target_link_libraries(evm_calculator_test ocudu_channel_modulation ocuduvec ocudulog GTest::gtest GTest::gtest_main)
add_test(evm_calculator_test evm_calculator_test)
