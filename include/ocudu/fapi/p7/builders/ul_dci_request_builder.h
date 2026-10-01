// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/dl_pdcch_pdu_builder.h"
#include "ocudu/fapi/p7/messages/ul_dci_request.h"

namespace ocudu {
namespace fapi {

/// UL_DCI.request message builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.4.
class ul_dci_request_builder
{
  ul_dci_request& msg;

public:
  explicit ul_dci_request_builder(ul_dci_request& msg_) : msg(msg_) {}

  /// \brief Sets the UL_DCI.request slot point and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.4 in table UL_DCI.request message body.
  ul_dci_request_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Adds a PDCCH PDU to the UL_DCI.request basic parameters and returns a reference to the PDCCH PDU builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.4 in table UL_DCI.request message body.
  dl_pdcch_pdu_builder add_pdcch_pdu()
  {
    auto& pdu = msg.pdus.emplace_back();

    dl_pdcch_pdu_builder builder(pdu.pdu);

    return builder;
  }
};

} // namespace fapi
} // namespace ocudu
