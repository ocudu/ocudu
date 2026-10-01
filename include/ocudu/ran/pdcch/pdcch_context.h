// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

/// PDCCH transmission context.
class pdcch_context
{
public:
  /// Default constructor.
  pdcch_context() = default;

  explicit pdcch_context(search_space_id         ss_id_,
                         const char*             dci_format_,
                         std::optional<unsigned> harq_feedback_timing_) :
    ss_id(ss_id_), dci_format(dci_format_), harq_feedback_timing(harq_feedback_timing_)
  {
  }

  /// Comparison operators.
  bool operator==(const pdcch_context& other) const
  {
    return ss_id == other.ss_id && dci_format == other.dci_format && harq_feedback_timing == other.harq_feedback_timing;
  }
  bool operator!=(const pdcch_context& other) const { return !(*this == other); }

private:
  friend struct fmt::formatter<pdcch_context>;
  search_space_id         ss_id      = MAX_NOF_SEARCH_SPACES;
  const char*             dci_format = nullptr;
  std::optional<unsigned> harq_feedback_timing;
};

} // namespace ocudu
