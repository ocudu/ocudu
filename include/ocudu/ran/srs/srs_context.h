// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"

namespace ocudu {

/// Sounding Reference Signals (SRS) reception context.
class srs_context
{
public:
  /// Default constructor.
  srs_context() = default;

  /// Constructs from values.
  explicit srs_context(unsigned sector_id_, rnti_t rnti_) : sector_id(sector_id_), rnti(rnti_) {}

private:
  friend struct fmt::formatter<srs_context>;
  unsigned sector_id = 0;
  rnti_t   rnti      = rnti_t::MAX_CRNTI;
};

} // namespace ocudu
