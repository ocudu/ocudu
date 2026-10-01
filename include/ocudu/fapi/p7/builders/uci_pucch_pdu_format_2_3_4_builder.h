// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/uci_pucch_pdu_format_2_3_4.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {
namespace fapi {

/// UCI PUSCH PDU Format 2, Format 3 or Format 4 builder that helps fill in the parameters specified in SCF-222 v4.0
/// Section 3.4.9.3.
class uci_pucch_pdu_format_2_3_4_builder
{
  uci_pucch_pdu_format_2_3_4& pdu;

public:
  explicit uci_pucch_pdu_format_2_3_4_builder(uci_pucch_pdu_format_2_3_4& pdu_) : pdu(pdu_) {}

  /// \brief Sets the UCI PUCCH Format 2, Format 3 and Format 4 PDU UE specific parameters and returns a reference to
  /// the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.3 in Table UCI PUCCH Format 2, Format
  /// 3 or Format 4 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the UCI PUCCH Format 2, Format 3 and Format 4 PDU UE format and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.3 in Table UCI PUCCH Format 2, Format
  /// 3 or Format 4 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_format(pucch_format type)
  {
    switch (type) {
      case pucch_format::FORMAT_2:
        pdu.pucch_format = uci_pucch_pdu_format_2_3_4::format_type::format_2;
        break;
      case pucch_format::FORMAT_3:
        pdu.pucch_format = uci_pucch_pdu_format_2_3_4::format_type::format_3;
        break;
      case pucch_format::FORMAT_4:
        pdu.pucch_format = uci_pucch_pdu_format_2_3_4::format_type::format_4;
        break;
      default:
        ocudu_assert(0, "PUCCH format={} is not supported by this PDU", fmt::underlying(type));
        break;
    }

    return *this;
  }

  /// \brief Sets the UCI PUCCH Format 2, Format 3 and Format 4 PDU metric parameters and returns a reference to the
  /// builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.3 in Table UCI PUCCH Format 2, Format
  /// 3 or Format 4 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_metrics_parameters(std::optional<float>           ul_sinr_metric_dB,
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

  /// \brief Sets the SR PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.3 in Table UCI PUCCH Format 2, Format 3 or
  /// Format 4 PDU.
  uci_pucch_pdu_format_2_3_4_builder&
  set_sr_parameters(const bounded_bitset<sr_pdu_format_2_3_4::MAX_SR_PAYLOAD_SIZE_BITS>& sr_payload)
  {
    pdu.sr = sr_pdu_format_2_3_4{.sr_payload = sr_payload};

    return *this;
  }

  /// \brief Sets the HARQ PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.3 in Table UCI PUCCH Format 2, Format 3 or
  /// Format 4 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_harq_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                                          units::bits             expected_bit_length,
                                                          const uci_payload_type& payload)
  {
    pdu.harq =
        uci_harq_pdu{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }

  /// \brief Sets the CSI Part 1 PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table CSI Part 1 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_csi_part1_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                                               units::bits             expected_bit_length,
                                                               const uci_payload_type& payload)
  {
    pdu.csi_part1 =
        uci_csi_part1{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }

  /// \brief Sets the CSI Part 2 PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table CSI Part 2 PDU.
  uci_pucch_pdu_format_2_3_4_builder& set_csi_part2_parameters(uci_pusch_or_pucch_f2_3_4_detection_status detection,
                                                               units::bits             expected_bit_length,
                                                               const uci_payload_type& payload)
  {
    pdu.csi_part2 =
        uci_csi_part2{.detection_status = detection, .expected_bit_length = expected_bit_length, .payload = payload};

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
