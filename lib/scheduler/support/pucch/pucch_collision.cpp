// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_collision.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"

using namespace ocudu;

namespace {

/// Represents the time-frequency grants of a PUCCH resource.
struct pucch_grants {
  struct hop {
    ofdm_symbol_range syms;
    prb_interval      prbs;

    bool operator==(const hop& other) const { return syms == other.syms and prbs == other.prbs; }
    bool operator!=(const hop& other) const { return not(*this == other); }

    bool overlaps(const hop& other) const { return syms.overlaps(other.syms) and prbs.overlaps(other.prbs); }
  };
  hop                first_hop;
  std::optional<hop> second_hop;

  pucch_grants(const pucch_resource& res)
  {
    // Compute time-frequency grants.
    if (res.second_hop_prb.has_value()) {
      // Intra-slot frequency hopping.
      const unsigned half = res.syms.length() / 2;
      first_hop           = {{res.syms.start(), res.syms.start() + half}, res.prbs()};
      second_hop          = {{res.syms.start() + half, res.syms.stop()},
                             prb_interval::start_and_len(*res.second_hop_prb, res.prbs().length())};
    } else {
      // No intra-slot frequency hopping.
      first_hop  = {res.syms, res.prbs()};
      second_hop = std::nullopt;
    }
  }

  bool operator==(const pucch_grants& other) const
  {
    return first_hop == other.first_hop and second_hop == other.second_hop;
  }
  bool operator!=(const pucch_grants& other) const { return not(*this == other); }

  /// Checks if this pucch_grants overlaps with another pucch_grants.
  bool overlaps(const pucch_grants& other) const
  {
    // Check if the first grant overlaps with any of the other's grants.
    if (first_hop.overlaps(other.first_hop) or
        (other.second_hop.has_value() and first_hop.overlaps(*other.second_hop))) {
      return true;
    }
    // Check if the second grant (if any) overlaps with any of the other's grants.
    if (second_hop.has_value() and (second_hop->overlaps(other.first_hop) or
                                    (other.second_hop.has_value() and second_hop->overlaps(*other.second_hop)))) {
      return true;
    }
    return false;
  }
};

} // namespace

unsigned ocudu::pucch_mux_idx(const pucch_resource& res)
{
  switch (res.format()) {
    case pucch_format::FORMAT_0:
      return std::get<pucch_resource::f0_config>(res.format_params).initial_cyclic_shift;
    case pucch_format::FORMAT_1: {
      // For Format 1, two sequences are orthogonal unless both the ICS and the time domain OCC are the same.
      const auto& f1 = std::get<pucch_resource::f1_config>(res.format_params);
      return f1.initial_cyclic_shift + f1.time_domain_occ * pucch_constants::f1::NOF_ICS;
    }
    case pucch_format::FORMAT_4:
      // For Format 4, the OCC index is mapped to a cyclic shift value, as per Table 6.4.1.3.3.1-1, TS 38.211.
      // Thus, resources with different OCC indices will never collide, even if they have different OCC lengths.
      // Therefore, we can use the OCC index directly as the multiplexing index.
      return static_cast<unsigned>(std::get<pucch_resource::f4_config>(res.format_params).occ_index);
    default:
      // Non multiplexed formats.
      return 0;
  }
}

bool ocudu::pucch_resources_collide(const pucch_resource& res1, const pucch_resource& res2)
{
  const pucch_grants grants1(res1);
  const pucch_grants grants2(res2);
  if (not grants1.overlaps(grants2)) {
    // Resources that do not overlap in time and frequency do not collide.
    return false;
  }

  if (res1.format() != res2.format()) {
    // Resources with different formats always collide if they overlap in time and frequency.
    return true;
  }

  if (grants1 != grants2) {
    // We can only make sure resources have orthogonal sequences if they have the same time/frequency allocation.
    return true;
  }

  // Resources with the same format and time/frequency grants only collide if they have the same multiplexing index.
  // Note: resources with Format 2/3 always collide as they are not multiplexed (multiplexing index is always 0).
  return pucch_mux_idx(res1) == pucch_mux_idx(res2);
}
