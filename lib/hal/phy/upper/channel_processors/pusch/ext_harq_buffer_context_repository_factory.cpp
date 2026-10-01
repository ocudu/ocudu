// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"

using namespace ocudu;
using namespace hal;

std::shared_ptr<ext_harq_buffer_context_repository>
ocudu::hal::create_ext_harq_buffer_context_repository(unsigned nof_codeblocks,
                                                      uint64_t ext_harq_buff_size,
                                                      bool     debug_mode)
{
  return std::make_shared<ext_harq_buffer_context_repository>(nof_codeblocks, ext_harq_buff_size, debug_mode);
}
