// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/fapi/p7/messages/rx_data_indication.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {
namespace fapi {

/// Rx_Data.indication message builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.7.
class rx_data_indication_builder
{
  rx_data_indication& msg;

public:
  explicit rx_data_indication_builder(rx_data_indication& msg_) : msg(msg_) {}

  /// \brief Sets the \e Rx_Data.indication slot and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.7 in table Rx_Data.indication message body.
  rx_data_indication_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Sets a PDU to the message and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.7 in table Rx_Data.indication message body.
  rx_data_indication_builder& set_pdu(rnti_t rnti, harq_id_t harq_id, span<const uint8_t> transport_block)
  {
    msg.pdu.rnti            = rnti;
    msg.pdu.harq_id         = harq_id;
    msg.pdu.transport_block = transport_block;

    return *this;
  }

  /// \brief Sets the rapid parameter and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.7 in table Rx_Data.indication message body.
  rx_data_indication_builder& set_rapid_parameter(uint8_t rapid)
  {
    msg.pdu.rapid = rapid;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
