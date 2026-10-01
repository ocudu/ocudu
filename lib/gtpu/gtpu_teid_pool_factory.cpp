// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/gtpu/gtpu_teid_pool_factory.h"
#include "gtpu_teid_pool_impl.h"

using namespace ocudu;

std::unique_ptr<gtpu_teid_pool> ocudu::create_gtpu_allocator(const gtpu_allocator_creation_request& msg)
{
  return std::make_unique<gtpu_teid_pool_impl>(msg.max_nof_teids, msg.teid_release_linger_time, msg.timers);
}
