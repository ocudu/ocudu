// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"

namespace ocudu {

/// PUCCH transmission context.
class pucch_context
{
public:
  /// Default constructor.
  pucch_context() = default;

  /// Constructs from value.
  explicit pucch_context(rnti_t rnti_) : rnti(rnti_) {}

private:
  friend struct fmt::formatter<pucch_context>;
  rnti_t rnti = rnti_t::INVALID_RNTI;
};

} // namespace ocudu
