// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/crc_indication.h"

namespace ocudu {
namespace fapi {

/// CRC.indication message builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.8.
class crc_indication_builder
{
  crc_indication& msg;

public:
  explicit crc_indication_builder(crc_indication& msg_) : msg(msg_) {}

  /// \brief Sets the \e CRC.indication slot and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.8 in table CRC.indication message body.
  crc_indication_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Sets a \e CRC.indication PDU to the message and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.8 in table CRC.indication message body.
  crc_indication_builder& set_pdu(rnti_t                         rnti,
                                  harq_id_t                      harq_id,
                                  bool                           tb_crc_status_ok,
                                  std::optional<float>           ul_sinr_metric_dB,
                                  std::optional<phy_time_unit>   timing_advance_offset,
                                  std::optional<fapi_power_unit> rssi,
                                  std::optional<fapi_power_unit> rsrp)
  {
    msg.pdu.rnti                  = rnti;
    msg.pdu.harq_id               = harq_id;
    msg.pdu.tb_crc_status_ok      = tb_crc_status_ok;
    msg.pdu.timing_advance_offset = timing_advance_offset;
    msg.pdu.rssi                  = rssi;
    msg.pdu.rsrp                  = rsrp;
    msg.pdu.ul_sinr_metric_dB     = ul_sinr_metric_dB;

    return *this;
  }

  /// \brief Sets the rapid parameter and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.8 in table CRC.indication message body.
  crc_indication_builder& set_rapid_parameter(uint8_t rapid)
  {
    msg.pdu.rapid = rapid;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
