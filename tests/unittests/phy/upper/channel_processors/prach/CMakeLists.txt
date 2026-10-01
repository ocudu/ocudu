# SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "phy")

if (ENABLE_CUDA)
    add_executable(prach_detector_cuda_test prach_detector_cuda_test.cpp)
    target_link_libraries(prach_detector_cuda_test
            ocudu_prach_detector
            ocudu_phy_support
            ocudu_dft
            ocudulog
            gtest
            gtest_main)
    add_cuda_test(prach_detector_cuda_test prach_detector_cuda_test)
endif (ENABLE_CUDA)
