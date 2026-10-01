// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include <map>
#include <optional>

namespace ocudu::ocucp {

/// Mapping of the TRPs the connected DUs host, learned from the TRP Information Exchange procedure,
/// TS 38.455 section 8.2.8.
class nrppa_trp_registry
{
public:
  /// Returns true when the TRP Information Exchange procedure has reported at least one TRP.
  ///
  /// TRP-scoped procedures address a TRP by its ID and cannot resolve a DU before the LMF has run the TRP Information
  /// Exchange procedure. They gate on this.
  [[nodiscard]] bool has_trp_information() const { return !trp_to_du.empty(); }

  /// Returns the index of the DU hosting the given TRP, or nullopt when the TRP is unknown.
  [[nodiscard]] std::optional<cu_cp_du_index_t> find_du(trp_id_t trp_id) const
  {
    auto it = trp_to_du.find(trp_id);
    if (it == trp_to_du.end()) {
      return std::nullopt;
    }
    return it->second;
  }

  /// Maps a TRP to the DU hosting it.
  void add_trp(trp_id_t trp_id, cu_cp_du_index_t du_index) { trp_to_du.emplace(trp_id, du_index); }

  /// Drops every TRP hosted by the given DU.
  void remove_du(cu_cp_du_index_t du_index)
  {
    for (auto it = trp_to_du.begin(); it != trp_to_du.end();) {
      it = it->second == du_index ? trp_to_du.erase(it) : std::next(it);
    }
  }

private:
  std::map<trp_id_t, cu_cp_du_index_t> trp_to_du;
};

} // namespace ocudu::ocucp
