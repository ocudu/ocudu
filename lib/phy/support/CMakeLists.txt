# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(OCUDU_PHY_SUPPORT_SOURCES
        interpolator/interpolator_linear_impl.cpp
        time_alignment_estimator/time_alignment_estimator_dft_impl.cpp
        time_alignment_estimator/time_alignment_estimator_factories.cpp
        rb_allocation.cpp
        re_pattern.cpp
        resource_grid_impl.cpp
        resource_grid_mapper_impl.cpp
        resource_grid_pool_impl.cpp
        resource_grid_reader_impl.cpp
        resource_grid_writer_impl.cpp
        support_factories.cpp)

# The CUDA-visible grid and PRACH buffer share one managed allocation between the host and the
# device, so they are only built when the build has CUDA support.
if (ENABLE_CUDA)
    list(APPEND OCUDU_PHY_SUPPORT_SOURCES
            prach_buffer_cuda_visible_impl.cpp
            prach_buffer_cuda_visible_factory.cpp
            resource_grid_cuda_visible_impl.cpp
            resource_grid_cuda_visible_factory.cpp)
else (ENABLE_CUDA)
    list(APPEND OCUDU_PHY_SUPPORT_SOURCES
            prach_buffer_cuda_visible_factory_unavailable.cpp
            resource_grid_cuda_visible_factory_unavailable.cpp)
endif (ENABLE_CUDA)

add_library(ocudu_phy_support STATIC ${OCUDU_PHY_SUPPORT_SOURCES})
target_link_libraries(ocudu_phy_support ocuduvec ocudu_instrumentation ocudu_ran ocudu_support_math)
if (ENABLE_CUDA)
    target_link_libraries(ocudu_phy_support ocudu_cuda_adt ocudu_cuda_support)
endif (ENABLE_CUDA)

add_to_exported_libs(ocudu_phy_support)
