// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/gtpu/gtpu_demux_factory.h"
#include "gtpu_demux_impl.h"

using namespace ocudu;

std::unique_ptr<gtpu_demux> ocudu::create_gtpu_demux(const gtpu_demux_creation_request& msg)
{
  return std::make_unique<gtpu_demux_impl>(msg.cfg,
                                           gtpu_demux_impl_dependencies{.teid_linger_checker = msg.teid_linger_checker,
                                                                        .gtpu_pcap           = msg.gtpu_pcap,
                                                                        .logger = ocudulog::fetch_basic_logger("GTPU"),
                                                                        .rate_limiter = msg.rate_limiter});
}
