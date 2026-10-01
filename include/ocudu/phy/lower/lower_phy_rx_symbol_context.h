// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// Describes the context of a newly received symbol.
struct lower_phy_rx_symbol_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
  /// The last processed symbol index within the slot.
  unsigned nof_symbols;
  /// Set to true if the received OFDM symbol contains valid information.
  bool is_valid;
};

} // namespace ocudu
