// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/fapi/p7/messages/uci_pucch_pdu_format_0_1.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/rnti.h"
#include "fmt/format.h"

namespace ocudu {
namespace fapi {

/// UCI PUCCH PDU Format 0 or Format 1 builder that helps fill in the parameters specified in SCF-222 v4.0
/// Section 3.4.9.2.
class uci_pucch_pdu_format_0_1_builder
{
  uci_pucch_pdu_format_0_1& pdu;

public:
  explicit uci_pucch_pdu_format_0_1_builder(uci_pucch_pdu_format_0_1& pdu_) : pdu(pdu_) {}

  /// \brief Sets the UCI PUCCH PDU Format 0 and Format 1 UE specific parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.2 in Table UCI PUCCH Format 0 or Format 1 PDU.
  uci_pucch_pdu_format_0_1_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the UCI PUCCH PDU Format 0 and Format 1 format and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.2 in Table UCI PUCCH Format 0 or Format 1 PDU.
  uci_pucch_pdu_format_0_1_builder& set_format(pucch_format type)
  {
    switch (type) {
      case pucch_format::FORMAT_0:
        pdu.pucch_format = uci_pucch_pdu_format_0_1::format_type::format_0;
        break;
      case pucch_format::FORMAT_1:
        pdu.pucch_format = uci_pucch_pdu_format_0_1::format_type::format_1;
        break;
      default:
        ocudu_assert(0, "PUCCH format={} is not supported by this PDU", fmt::underlying(type));
        break;
    }

    return *this;
  }

  /// \brief Sets the UCI PUCCH PDU Format 0 and Format 1 time advance and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.2 in Table UCI PUCCH Format 0 or Format 1 PDU.
  uci_pucch_pdu_format_0_1_builder& set_time_advance(std::optional<phy_time_unit> timing_advance_offset)
  {
    pdu.timing_advance_offset = timing_advance_offset;

    return *this;
  }

  /// \brief Sets the UCI PUCCH PDU Format 0 and Format 1 power parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.2 in Table UCI PUCCH Format 0 or Format 1 PDU.
  uci_pucch_pdu_format_0_1_builder& set_metrics_parameters(std::optional<float>           ul_sinr_metric_dB,
                                                           std::optional<fapi_power_unit> rssi,
                                                           std::optional<fapi_power_unit> rsrp)
  {
    pdu.ul_sinr_metric_dB = ul_sinr_metric_dB;
    pdu.rssi              = rssi;
    pdu.rsrp              = rsrp;

    return *this;
  }

  /// \brief Sets the SR PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.4 in Table SR PDU for Format 0 or Format 1 PDU.
  uci_pucch_pdu_format_0_1_builder& set_sr_parameters(bool detected)
  {
    pdu.sr = sr_pdu_format_0_1{.sr_detected = detected};

    return *this;
  }

  /// \brief Sets the HARQ PDU parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9.2 in Table HARQ PDU for Format 0 or Format 1
  /// PDU.
  uci_pucch_pdu_format_0_1_builder& set_harq_parameters(span<const uci_pucch_f0_or_f1_harq_values> value)
  {
    pdu.harq = uci_harq_format_0_1{.harq_values = {value.begin(), value.end()}};

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
