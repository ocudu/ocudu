// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

/// PDSCH transmission context.
class pdsch_context
{
public:
  /// Default constructor.
  pdsch_context() = default;

  /// Constructs from values.
  explicit pdsch_context(harq_id_t h_id_, unsigned k1_, unsigned nof_retxs_) :
    h_id(h_id_), k1(k1_), nof_retxs(nof_retxs_)
  {
  }

  /// Comparison operators.
  bool operator==(const pdsch_context& other) const
  {
    return h_id == other.h_id && k1 == other.k1 && nof_retxs == other.nof_retxs;
  }
  bool operator!=(const pdsch_context& other) const { return !(*this == other); }

  /// Gets the HARQ process identifier.
  harq_id_t get_h_id() const { return h_id; }

  /// Gets if the PDSCH transmission is new.
  bool is_new_data() const { return nof_retxs == 0; }

private:
  friend struct fmt::formatter<pdsch_context>;
  harq_id_t h_id      = INVALID_HARQ_ID;
  unsigned  k1        = 0;
  unsigned  nof_retxs = 0;
};

} // namespace ocudu
