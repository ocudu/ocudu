// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/fapi/p7/messages/uci_pdu_definitions.h"
#include "ocudu/ran/phy_time_unit.h"

namespace ocudu {
namespace fapi {

/// PUSCH UCI PDU information.
struct uci_pusch_pdu {
  uint32_t                       handle = 0U;
  rnti_t                         rnti;
  std::optional<float>           ul_sinr_metric_dB;
  std::optional<phy_time_unit>   timing_advance_offset;
  std::optional<fapi_power_unit> rssi;
  std::optional<fapi_power_unit> rsrp;
  std::optional<uci_harq_pdu>    harq;
  std::optional<uci_csi_part1>   csi_part1;
  std::optional<uci_csi_part2>   csi_part2;
};

} // namespace fapi
} // namespace ocudu
