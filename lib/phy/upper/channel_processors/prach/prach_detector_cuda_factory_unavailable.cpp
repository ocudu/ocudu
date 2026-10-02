// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

// Built in place of prach_detector_cuda_factory.cpp when the build has no CUDA support, so that the
// callers select the accelerated detector through the same declaration no matter if the build has CUDA support.

#include "ocudu/phy/upper/channel_processors/prach/factories.h"

using namespace ocudu;

std::shared_ptr<prach_detector_factory>
ocudu::create_prach_detector_factory_cuda(std::shared_ptr<prach_detector_factory> /* fallback_factory */,
                                          std::shared_ptr<prach_generator_factory> /* generator_factory */,
                                          const prach_detector_cuda_configuration& /* config */)
{
  return nullptr;
}
