// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pucch/pucch_uci_message.h"
#include "ocudu/phy/upper/channel_state_information.h"

namespace ocudu {

/// Collects PUCCH processor results.
struct pucch_processor_result {
  /// Channel state information.
  channel_state_information csi;
  /// UCI message.
  pucch_uci_message message;
  /// Detection metric normalized with respect to the detection threshold (if applicable).
  std::optional<float> detection_metric;
};

} // namespace ocudu
