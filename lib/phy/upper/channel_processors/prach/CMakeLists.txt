# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(OCUDU_PRACH_DETECTOR_SOURCES
        prach_detector_generic_impl.cpp
        prach_detector_generic_thresholds.cpp
        prach_generator_impl.cpp
        prach_detector_phy_validator_impl.cpp
        factories.cpp)

# The accelerated detector is selected through the same declaration whether or not the build has
# CUDA support, so one of the two factory implementations is always present.
if (ENABLE_CUDA)
    list(APPEND OCUDU_PRACH_DETECTOR_SOURCES
            prach_detector_cuda_impl.cpp
            prach_detector_cuda_factory.cpp)
else (ENABLE_CUDA)
    list(APPEND OCUDU_PRACH_DETECTOR_SOURCES prach_detector_cuda_factory_unavailable.cpp)
endif (ENABLE_CUDA)

add_library(ocudu_prach_detector STATIC ${OCUDU_PRACH_DETECTOR_SOURCES})
target_link_libraries(ocudu_prach_detector ocudu_ran ocuduvec)

if (ENABLE_CUDA)
    target_link_libraries(ocudu_prach_detector ocudu_cuda_prach ocudu_cuda_adt)
endif (ENABLE_CUDA)
