# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(cuda_adt_test cuda_adt_test.cpp)
target_link_libraries(cuda_adt_test ocudu_cuda_adt ocudu_cuda_support ocudulog gtest gtest_main)
# The CUDA runtime rejects allocations with cudaErrorDevicesUnavailable while several exclusive
# device contexts are alive, so the tests that open one are serialised against each other.
add_test(cuda_adt_test cuda_adt_test)
set_tests_properties(cuda_adt_test PROPERTIES RESOURCE_LOCK ocudu_cuda_device)
