# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        prach_detector_backend.cu)

add_library(ocudu_cuda_prach ${SOURCES})

# VkFFT selects its backend at compile time and has no default; 1 is the CUDA backend
target_compile_definitions(ocudu_cuda_prach PRIVATE VKFFT_BACKEND=1)
target_include_directories(ocudu_cuda_prach PRIVATE ${VkFFT_INCLUDE_DIRS})

# The inverse DFT plan is built through the CUDA driver API, which VkFFT takes a device handle from,
# and VkFFT compiles the transform for the target GPU at run time through NVRTC
target_link_libraries(ocudu_cuda_prach ocudu_cuda_adt ocudu_support CUDA::cudart CUDA::cuda_driver CUDA::nvrtc)

add_to_exported_libs(ocudu_cuda_prach)
