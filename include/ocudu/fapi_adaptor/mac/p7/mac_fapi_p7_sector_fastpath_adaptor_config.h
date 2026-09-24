// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/precoding_codebook_mapper.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <memory>

namespace ocudu {

class task_executor;

namespace fapi {
class p7_last_request_notifier;
class p7_requests_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// MAC-FAPI P7 sector fastpath adaptor configuration.
struct mac_fapi_p7_sector_fastpath_adaptor_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Cell number of resource blocks.
  unsigned cell_nof_prbs;
  /// Subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs;
};

/// MAC-FAPI P7 sector fastpath adaptor dependencies.
struct mac_fapi_p7_sector_fastpath_adaptor_dependencies {
  /// FAPI P7 requests gateway.
  fapi::p7_requests_gateway& p7_gateway;
  /// P7 last requests notifier.
  fapi::p7_last_request_notifier& p7_last_req_notifier;
  /// Precoding codebook mapper.
  std::unique_ptr<precoding_codebook_mapper> pm_mapper;
  /// UCI Part2 mapper.
  std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper;
  /// FAPI logger.
  ocudulog::basic_logger& fapi_logger;
};

} // namespace fapi_adaptor
} // namespace ocudu
