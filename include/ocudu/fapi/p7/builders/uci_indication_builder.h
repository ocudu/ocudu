// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/uci_pucch_pdu_format_0_1_builder.h"
#include "ocudu/fapi/p7/builders/uci_pucch_pdu_format_2_3_4_builder.h"
#include "ocudu/fapi/p7/builders/uci_pusch_pdu_builder.h"
#include "ocudu/fapi/p7/messages/uci_indication.h"

namespace ocudu {
namespace fapi {

/// UCI.indication message builder that helps to fill in the parameters specified in SCF-222 v4.0 Section 3.4.9.
class uci_indication_builder
{
  uci_indication& msg;

public:
  explicit uci_indication_builder(uci_indication& msg_) : msg(msg_) {}

  /// \brief Sets the \e UCI.indication slot point and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9 in table UCI.indication message body.
  uci_indication_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Adds a PUSCH PDU to the \e UCI.indication message and returns a PUSCH PDU builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9 in table UCI.indication message body.
  uci_pusch_pdu_builder add_pusch_pdu()
  {
    ocudu_assert(msg.pdu.index() == 0 || std::holds_alternative<uci_pusch_pdu>(msg.pdu),
                 "Builder of PDU already called for a different PDU: {}",
                 msg.pdu.index());

    auto& pusch_pdu = msg.pdu.emplace<uci_pusch_pdu>();

    uci_pusch_pdu_builder builder(pusch_pdu);

    return builder;
  }

  /// \brief Adds a PUCCH Format 0 and Format 1 PDU to the \e UCI.indication message and returns a PUCCH Format 0 and
  /// Format 1 PDU builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9 in table UCI.indication message body.
  uci_pucch_pdu_format_0_1_builder add_format_0_1_pucch_pdu()
  {
    ocudu_assert(msg.pdu.index() == 0 || std::holds_alternative<uci_pucch_pdu_format_0_1>(msg.pdu),
                 "Builder of PDU already called for a different PDU: {}",
                 msg.pdu.index());

    auto& format_0_1_pdu = msg.pdu.emplace<uci_pucch_pdu_format_0_1>();

    uci_pucch_pdu_format_0_1_builder builder(format_0_1_pdu);

    return builder;
  }

  /// \brief Adds a PUCCH Format 2, Format 3 or Format 4 PDU to the \e UCI.indication message and returns a PUCCH
  /// Format 2, Format 3 and Format 4 PDU builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 Section 3.4.9 in table UCI.indication message body.
  uci_pucch_pdu_format_2_3_4_builder add_format_2_3_4_pucch_pdu()
  {
    ocudu_assert(msg.pdu.index() == 0 || std::holds_alternative<uci_pucch_pdu_format_2_3_4>(msg.pdu),
                 "Builder of PDU already called for a different PDU: {}",
                 msg.pdu.index());

    auto& format_2_3_4_pdu = msg.pdu.emplace<uci_pucch_pdu_format_2_3_4>();

    uci_pucch_pdu_format_2_3_4_builder builder(format_2_3_4_pdu);

    return builder;
  }
};

} // namespace fapi
} // namespace ocudu
