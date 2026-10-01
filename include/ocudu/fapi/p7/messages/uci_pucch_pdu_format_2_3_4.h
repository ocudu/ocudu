// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/fapi/p7/messages/uci_pdu_definitions.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {
namespace fapi {

/// SR PDU for format 2, 3, or 4.
struct sr_pdu_format_2_3_4 {
  /// Maximum number of supported SR bits in this message.
  static constexpr unsigned MAX_SR_PAYLOAD_SIZE_BITS = 4;

  bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS> sr_payload;
};

/// UCI PUCCH for format 2, 3, or 4.
struct uci_pucch_pdu_format_2_3_4 {
  enum class format_type : uint8_t { format_2, format_3, format_4 };

  uint32_t                           handle = 0U;
  rnti_t                             rnti;
  format_type                        pucch_format;
  std::optional<float>               ul_sinr_metric_dB;
  std::optional<phy_time_unit>       timing_advance_offset;
  std::optional<fapi_power_unit>     rssi;
  std::optional<fapi_power_unit>     rsrp;
  std::optional<sr_pdu_format_2_3_4> sr;
  std::optional<uci_harq_pdu>        harq;
  std::optional<uci_csi_part1>       csi_part1;
  std::optional<uci_csi_part2>       csi_part2;
};

} // namespace fapi
} // namespace ocudu
