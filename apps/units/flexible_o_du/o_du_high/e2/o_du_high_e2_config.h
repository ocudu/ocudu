// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_appconfig.h"

namespace ocudu {

/// O-RAN DU high E2 PCAP configuration.
struct o_du_high_e2_pcap_config {
  std::string filename = "/tmp/du_e2ap.pcap";
  bool        enabled  = false;
};

/// O-RAN DU high configuration.
struct o_du_high_e2_config {
  /// E2 configuration.
  e2_config base_cfg;
  /// PCAPs.
  o_du_high_e2_pcap_config pcaps;
};

} // namespace ocudu
