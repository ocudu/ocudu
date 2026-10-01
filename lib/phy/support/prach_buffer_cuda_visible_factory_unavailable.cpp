// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

// Definition used when the build has no CUDA support. It always returns nullptr so callers can
// call create_prach_buffer_cuda_visible() unconditionally and check the result, instead of guarding
// the call with #ifdef.

#include "ocudu/phy/support/support_factories.h"

using namespace ocudu;

std::unique_ptr<prach_buffer>
ocudu::create_prach_buffer_cuda_visible(unsigned                                       max_nof_antennas,
                                        unsigned                                       max_nof_td_occasions,
                                        unsigned                                       max_nof_fd_occasions,
                                        bool                                           is_long_preamble,
                                        const cuda_visible_prach_buffer_configuration& config)
{
  (void)max_nof_antennas;
  (void)max_nof_td_occasions;
  (void)max_nof_fd_occasions;
  (void)is_long_preamble;
  (void)config;
  return nullptr;
}
