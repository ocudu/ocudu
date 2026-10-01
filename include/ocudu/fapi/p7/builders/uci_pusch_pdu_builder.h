// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/uci_pusch_pdu.h"

namespace ocudu {
namespace fapi {

/// UCI PUSCH PDU builder that helps fill in the parameters specified in SCF-222 v4.0 section 3.4.9.1.
class uci_pusch_pdu_builder
{
  uci_pusch_pdu& pdu;

public:
  explicit uci_pusch_pdu_builder(uci_pusch_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the UCI PUSCH PDU UE specific parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.1 in Table UCI PUSCH PDU.
  uci_pusch_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the UCI PUSCH PDU metrics parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.1 in Table UCI PUSCH PDU.
  uci_pusch_pdu_builder& set_metrics_parameters(std::optional<float>           ul_sinr_metric_dB,
                                                std::optional<phy_time_unit>   timing_advance_offset,
                                                std::optional<fapi_power_unit> rssi,
                                                std::optional<fapi_power_unit> rsrp)
  {
    pdu.timing_advance_offset = timing_advance_offset;
    pdu.ul_sinr_metric_dB     = ul_sinr_metric_dB;
    pdu.rssi                  = rssi;
    pdu.rsrp                  = rsrp;

    return *this;
  }

  /// \brief Sets the HARQ PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table HARQ PDU for Format 2, Format 3 or
  /// Format 4 or for PUSCH.
  uci_pusch_pdu_builder& set_harq_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                             units::bits                                expected_bit_length,
                                             const uci_payload_type&                    payload)
  {
    pdu.harq =
        uci_harq_pdu{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }

  /// \brief Sets the CSI Part 1 PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table CSI Part 1 PDU.
  uci_pusch_pdu_builder& set_csi_part1_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                                  units::bits                                expected_bit_length,
                                                  const uci_payload_type&                    payload)
  {
    pdu.csi_part1 =
        uci_csi_part1{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }

  /// \brief Sets the CSI Part 2 PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table CSI Part 2 PDU.
  uci_pusch_pdu_builder& set_csi_part2_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                                  units::bits                                expected_bit_length,
                                                  const uci_payload_type&                    payload)
  {
    pdu.csi_part2 =
        uci_csi_part2{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
