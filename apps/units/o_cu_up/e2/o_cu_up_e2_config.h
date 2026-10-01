// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_appconfig.h"

namespace ocudu {

/// O-RAN CU-UP E2 PCAP config.
struct o_cu_up_e2_pcap_config {
  std::string filename = "/tmp/cu_up_e2ap.pcap";
  bool        enabled  = false;
};

/// O-RAN CU-UP E2 configuration.
struct o_cu_up_e2_config {
  e2_config              base_config;
  o_cu_up_e2_pcap_config pcaps;
};

} // namespace ocudu
