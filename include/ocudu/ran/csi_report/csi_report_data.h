// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "csi_report_configuration.h"
#include "ocudu/adt/bounded_integer.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include <optional>
#include <variant>

namespace ocudu {

/// Channel Quality Indicator value.
using cqi_value = bounded_integer<uint8_t, 0, 15>;

/// Channel Quality Indicator type.
using csi_report_wideband_cqi_type = cqi_value;

/// Channel State Information (CSI) report fields.
struct csi_report_data {
  /// Rank Indicator (RI) data type.
  using ri_type = bounded_integer<uint8_t, 1, 8>;
  /// Layer Indicator (LI) data type.
  using li_type = bounded_integer<uint8_t, 0, 7>;
  /// Wideband Channel Quality Indicator (CQI) data type.
  using wideband_cqi_type = bounded_integer<uint8_t, 0, 15>;
  /// Subband differential Channel Quality Indicator (CQI) data type.
  using subband_diff_cqi_type = bounded_integer<uint8_t, 0, 3>;
  /// Subband differential CQI collection data type.
  using subband_diff_cqi_list = static_vector<subband_diff_cqi_type, csi_max_nof_subbands>;

  /// CSI-RS Resource Indicator (CRI) for each reported CSI-RS resource if reported.
  static_vector<uint8_t, csi_max_nof_reported_rs> cri;
  /// Reference Signal Received Power (RSRP) in dBm for each reported CSI-RS resource if reported. The range is
  /// [-140, -44] dBm.
  static_vector<int, csi_max_nof_reported_rs> rsrp_dBm;
  /// Rank Indicator (RI) if reported. The range is {1, ..., 8}.
  std::optional<ri_type> ri;
  /// Layer Indicator (LI) if reported.
  std::optional<li_type> li;
  /// PMI wideband information fields if reported.
  std::optional<precoding_matrix_indicator> pmi;
  /// Wideband CQI for the first TB.
  std::optional<wideband_cqi_type> first_tb_wideband_cqi;
  /// Wideband CQI for the second TB.
  std::optional<wideband_cqi_type> second_tb_wideband_cqi;
  /// Subband differential CQI for the first TB.
  std::optional<subband_diff_cqi_list> first_tb_subband_diff_cqi;
  /// Subband differential CQI for the second TB.
  std::optional<subband_diff_cqi_list> second_tb_subband_diff_cqi;
  /// Flag indicating if the CSI was detected correctly.
  bool valid;
};

} // namespace ocudu
