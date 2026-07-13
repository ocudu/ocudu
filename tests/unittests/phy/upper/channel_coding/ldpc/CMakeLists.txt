# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(ldpc_chain_test ldpc_chain_test.cpp)
target_link_libraries(ldpc_chain_test ocudu_channel_coding ocudulog GTest::gtest GTest::gtest_main)
add_test(ldpc_chain_test ldpc_chain_test)

add_executable(ldpc_rate_dematcher_test ldpc_rate_dematcher_test.cpp)
target_link_libraries(ldpc_rate_dematcher_test ocudu_channel_coding ocudulog GTest::gtest GTest::gtest_main)
add_test(ldpc_rate_dematcher_test ldpc_rate_dematcher_test)
