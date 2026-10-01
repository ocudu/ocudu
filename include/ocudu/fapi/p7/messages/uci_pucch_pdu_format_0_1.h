// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/uci/uci_mapping.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// SR PDU for format 0 or 1.
struct sr_pdu_format_0_1 {
  bool sr_detected;
};

/// UCI HARQ PDU for format 0 or 1.
struct uci_harq_format_0_1 {
  /// Maximum number of HARQs.
  static constexpr unsigned MAX_NUM_HARQ = 2U;

  static_vector<uci_pucch_f0_or_f1_harq_values, MAX_NUM_HARQ> harq_values;
};

/// UCI PUCCH for format 0 or 1.
struct uci_pucch_pdu_format_0_1 {
  enum class format_type : uint8_t { format_0, format_1 };

  uint32_t                           handle = 0U;
  rnti_t                             rnti;
  format_type                        pucch_format;
  std::optional<float>               ul_sinr_metric_dB;
  std::optional<phy_time_unit>       timing_advance_offset;
  std::optional<fapi_power_unit>     rssi;
  std::optional<fapi_power_unit>     rsrp;
  std::optional<sr_pdu_format_0_1>   sr;
  std::optional<uci_harq_format_0_1> harq;
};

} // namespace fapi
} // namespace ocudu
