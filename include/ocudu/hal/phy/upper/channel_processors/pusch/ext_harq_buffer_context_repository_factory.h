// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include <memory>

namespace ocudu {
namespace hal {

/// Returns a ext_harq_buffer_context_repository instance on success, otherwise returns nullptr.
std::shared_ptr<ext_harq_buffer_context_repository>
create_ext_harq_buffer_context_repository(unsigned nof_codeblocks,
                                          uint64_t ext_harq_buff_size,
                                          bool     debug_mode = false);

} // namespace hal
} // namespace ocudu
