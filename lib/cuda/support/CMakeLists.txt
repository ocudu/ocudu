# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        cuda_device.cpp)

add_library(ocudu_cuda_support ${SOURCES})

target_link_libraries(ocudu_cuda_support ocudu_support CUDA::cudart)

add_to_exported_libs(ocudu_cuda_support)
