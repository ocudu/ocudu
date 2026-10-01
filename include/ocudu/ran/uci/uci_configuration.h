// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/adt/tiny_optional.h"
#include <cstdint>
#include <variant>

namespace ocudu {

/// \c scaling values options, part of \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
enum class alpha_scaling_opt { f0p5, f0p65, f0p8, f1 };

inline float alpha_scaling_to_float(alpha_scaling_opt val)
{
  switch (val) {
    case alpha_scaling_opt::f0p5:
      return 0.5f;
    case alpha_scaling_opt::f0p65:
      return 0.65f;
    case alpha_scaling_opt::f0p8:
      return 0.8f;
    case alpha_scaling_opt::f1:
      return 1.0f;
    default:
      // NOTE: This is not in the specs, but it is to prevent the compiler from flagging a warning for reaching the end
      // of a non-void function without return statement.
      ocudu_assert(val > alpha_scaling_opt::f1, "Invalid Alpha scaling option");
      return 1.0f;
  }
}

/// \c BetaOffsets, as per TS 38.331. These values are mapped into floats as per Tables in Section 9.3, TS 38.213.
struct beta_offsets {
  /// \remark Apply to all members. For values larger than 31 mean, treat the parameter as "not present".
  static constexpr uint8_t invalid_beta_offset = 32;

  using uint_opt = tiny_optional<uint8_t, invalid_beta_offset>;

  /// Values {0,...,15}, as per Table 9.3-1, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {16,...,31} are accepted but treated as "reserved".
  uint_opt beta_offset_ack_idx_1;
  uint_opt beta_offset_ack_idx_2;
  uint_opt beta_offset_ack_idx_3;
  /// Values {0,...,18}, as per Table 9.3-2, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {19,...,31} are accepted but treated as "reserved".
  uint_opt beta_offset_csi_p1_idx_1;
  uint_opt beta_offset_csi_p1_idx_2;
  uint_opt beta_offset_csi_p2_idx_1;
  uint_opt beta_offset_csi_p2_idx_2;

  bool operator==(const beta_offsets& rhs) const
  {
    return beta_offset_ack_idx_1 == rhs.beta_offset_ack_idx_1 and beta_offset_ack_idx_2 == rhs.beta_offset_ack_idx_2 and
           beta_offset_ack_idx_3 == rhs.beta_offset_ack_idx_3 and
           beta_offset_csi_p1_idx_1 == rhs.beta_offset_csi_p1_idx_1 and
           beta_offset_csi_p1_idx_2 == rhs.beta_offset_csi_p1_idx_2 and
           beta_offset_csi_p2_idx_1 == rhs.beta_offset_csi_p2_idx_1 and
           beta_offset_csi_p2_idx_2 == rhs.beta_offset_csi_p2_idx_2;
  }
  bool operator!=(const beta_offsets& rhs) const { return !(rhs == *this); }
};

/// \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
struct uci_on_pusch {
  static constexpr size_t max_nof_beta_offsets = 4;

  using beta_offsets_semi_static = beta_offsets;
  using beta_offsets_dynamic     = static_vector<beta_offsets, max_nof_beta_offsets>;

  std::optional<std::variant<beta_offsets_dynamic, beta_offsets_semi_static>> beta_offsets_cfg;
  /// This value is not used for Configured Grant.
  alpha_scaling_opt scaling = alpha_scaling_opt::f1;

  bool operator==(const uci_on_pusch& rhs) const
  {
    return beta_offsets_cfg == rhs.beta_offsets_cfg and scaling == rhs.scaling;
  }

  bool operator!=(const uci_on_pusch& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
