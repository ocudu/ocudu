# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(crc_kernel_test crc_kernel_test.cpp)
target_link_libraries(crc_kernel_test
        ocudu_cuda_channel_coding
        ocudu_cuda_adt
        ocudu_channel_coding
        ocudulog
        GTest::gtest
        GTest::gtest_main)
add_cuda_test(crc_kernel_test crc_kernel_test)
