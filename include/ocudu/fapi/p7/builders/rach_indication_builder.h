// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/rach_indication.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// RACH.indication PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.11.
class rach_indication_pdu_builder
{
  rach_indication_pdu& pdu;

public:
  explicit rach_indication_pdu_builder(rach_indication_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the time domain parameters of the \e RACH.indication PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  rach_indication_pdu_builder& set_time_domain_parameters(uint8_t symbol_index, uint8_t slot_index)

  {
    pdu.symbol_index = symbol_index;
    pdu.slot_index   = slot_index;

    return *this;
  }

  /// \brief Sets the frequency domain parameters of the \e RACH.indication PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  rach_indication_pdu_builder& set_frequency_domain_parameters(uint8_t ra_index)
  {
    pdu.ra_index = ra_index;

    return *this;
  }

  /// \brief Sets the power parameters of the \e RACH.indication PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  rach_indication_pdu_builder& set_power_parameters(std::optional<fapi_power_unit> avg_rssi,
                                                    std::optional<float>           avg_snr_dB)

  {
    pdu.avg_rssi   = avg_rssi;
    pdu.avg_snr_dB = avg_snr_dB;

    return *this;
  }

  /// \brief Adds a preamble to the \e RACH.indication PDU and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  /// Units for timing advace offset parameter are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication
  /// message body, and this function expect these units.
  rach_indication_pdu_builder& add_preamble(unsigned                       preamble_index,
                                            std::optional<phy_time_unit>   timing_advance_offset,
                                            std::optional<fapi_power_unit> preamble_power,
                                            std::optional<float>           preamble_snr_dB)

  {
    auto& preamble = pdu.preambles.emplace_back();

    preamble.preamble_index        = preamble_index;
    preamble.timing_advance_offset = timing_advance_offset;
    preamble.preamble_pwr          = preamble_power;
    preamble.preamble_snr_dB       = preamble_snr_dB;

    return *this;
  }
};

/// \e RACH.indication message builder that helps to fill in the parameters specified in SCF-222 v4.0 Section 3.4.11.
class rach_indication_builder
{
  rach_indication& msg;

public:
  explicit rach_indication_builder(rach_indication& msg_) : msg(msg_) {}

  /// \brief Sets the slot of the \e RACH.indication message and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  rach_indication_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Sets a PDU to the \e RACH.indication message and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.11 in table RACH.indication message body.
  rach_indication_pdu_builder set_pdu(uint8_t                        symbol_index,
                                      uint8_t                        slot_index,
                                      uint8_t                        ra_index,
                                      std::optional<fapi_power_unit> avg_rssi,
                                      std::optional<float>           avg_snr_dB)
  {
    rach_indication_pdu_builder builder(msg.pdu);

    builder.set_time_domain_parameters(symbol_index, slot_index)
        .set_frequency_domain_parameters(ra_index)
        .set_power_parameters(avg_rssi, avg_snr_dB);

    return builder;
  }
};

} // namespace fapi
} // namespace ocudu
