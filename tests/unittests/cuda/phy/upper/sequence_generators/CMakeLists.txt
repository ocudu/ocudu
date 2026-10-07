# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(scrambling_kernel_test scrambling_kernel_test.cpp)
target_link_libraries(scrambling_kernel_test
        ocudu_cuda_sequence_generators
        ocudu_cuda_adt
        ocudu_sequence_generators
        ocudulog
        GTest::gtest
        GTest::gtest_main)
add_cuda_test(scrambling_kernel_test scrambling_kernel_test)
