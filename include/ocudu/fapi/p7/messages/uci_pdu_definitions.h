// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/uci/uci_payload_type.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace fapi {

/// UCI CSI Part 1 information.
struct uci_csi_part1 {
  uci_pusch_or_pucch_f2_3_4_detection_status detection_status;
  units::bits                                expected_bit_length;
  uci_payload_type                           payload;
};

/// UCI CSI Part 2 information.
struct uci_csi_part2 {
  uci_pusch_or_pucch_f2_3_4_detection_status detection_status;
  units::bits                                expected_bit_length;
  uci_payload_type                           payload;
};

/// UCI HARQ PDU information.
struct uci_harq_pdu {
  uci_pusch_or_pucch_f2_3_4_detection_status detection_status;
  units::bits                                expected_bit_length;
  uci_payload_type                           payload;
};

} // namespace fapi
} // namespace ocudu
