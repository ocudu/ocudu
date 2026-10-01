// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// RSRP value range in dBm used in RSRP measurements and thresholds, as specified in TS 38.331, "RSRP Range" and in
/// TS 38.133, Table 10.1.6.1-1.
/// For measurements, the value -156 means "<-156", the value -155 means between [-156, -155), and so on, until -30
/// which means >=-31. Value -29 is not used for measurements.
/// For RSRP thresholds, the value -29 represents infinity.
struct rsrp_range {
  rsrp_range() = default;
  explicit rsrp_range(int dBm) : val(dBm + 156) { ocudu_assert(dBm >= -156 and dBm <= -29, "Invalid RSRP dBm value"); }

  /// RSRP in dBm, ranging between [-156, -29].
  int16_t dBm() const { return static_cast<int16_t>(val) - 156; }

  /// Representation of RSRP with an index ranging between [0, 29] for the Table 10.1.6.1-1, TS 38.133.
  uint8_t count() const { return val; }

  /// Infinity RSRP value.
  static rsrp_range infinity() { return rsrp_range{-29}; }

  static rsrp_range min() { return rsrp_range{-156}; }
  static rsrp_range max() { return rsrp_range{-28}; }

  bool operator==(const rsrp_range& other) const { return val == other.val; }
  bool operator!=(const rsrp_range& other) const { return val != other.val; }
  bool operator<(const rsrp_range& other) const { return val < other.val; }
  bool operator>(const rsrp_range& other) const { return val > other.val; }
  bool operator<=(const rsrp_range& other) const { return val <= other.val; }
  bool operator>=(const rsrp_range& other) const { return val >= other.val; }

private:
  uint8_t val{0};
};

} // namespace ocudu
