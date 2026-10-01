# SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        cuda_copy.cpp
        cuda_error.cpp
        cuda_event.cpp
        cuda_stream.cpp
        device_vector.cpp
        managed_vector.cpp)

add_library(ocudu_cuda_adt ${SOURCES})

target_link_libraries(ocudu_cuda_adt ocudu_support CUDA::cudart)

add_to_exported_libs(ocudu_cuda_adt)
