// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include <memory>

namespace ocudu {

struct gtpu_demux_creation_request {
  gtpu_demux_cfg_t               cfg                 = {};
  gtpu_teid_lingering_interface* teid_linger_checker = nullptr;
  dlt_pcap*                      gtpu_pcap           = nullptr;
  lockfree_token_bucket*         rate_limiter        = nullptr;
};

/// Creates an instance of an GTP-U demux object.
std::unique_ptr<gtpu_demux> create_gtpu_demux(const gtpu_demux_creation_request& msg);

} // namespace ocudu
