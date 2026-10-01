// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/ul_prach_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pucch_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pusch_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_srs_pdu_builder.h"
#include "ocudu/fapi/p7/messages/ul_tti_request.h"
#include "ocudu/ran/pucch/pucch_mapping.h"

namespace ocudu {
namespace fapi {

/// UL_TTI.request message builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.
class ul_tti_request_builder
{
  ul_tti_request& msg;

public:
  explicit ul_tti_request_builder(ul_tti_request& msg_) : msg(msg_) {}

  /// \brief Sets the UL_TTI.request slot and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3 in table UL_TTI,request message body.
  ul_tti_request_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// \brief Adds a PRACH PDU to the message and returns a builder that helps to fill the parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
  ul_prach_pdu_builder add_prach_pdu()
  {
    auto&                pdu = msg.pdus.emplace_back();
    ul_prach_pdu_builder builder(pdu.pdu.emplace<ul_prach_pdu>());

    return builder;
  }

  /// Adds a PUCCH PDU to the message with the given format type and returns a builder that helps to fill the
  /// parameters.
  ul_pucch_pdu_builder add_pucch_pdu(pucch_format format_type)
  {
    auto& pdu          = msg.pdus.emplace_back();
    auto& emplaced_pdu = pdu.pdu.emplace<ul_pucch_pdu>();

    ul_pucch_pdu_builder builder(emplaced_pdu);

    switch (format_type) {
      case pucch_format::FORMAT_0:
        emplaced_pdu.format.emplace<ul_pucch_pdu_format_0>();
        break;
      case pucch_format::FORMAT_1:
        emplaced_pdu.format.emplace<ul_pucch_pdu_format_1>();
        break;
      case pucch_format::FORMAT_2:
        emplaced_pdu.format.emplace<ul_pucch_pdu_format_2>();
        break;
      case pucch_format::FORMAT_3:
        emplaced_pdu.format.emplace<ul_pucch_pdu_format_3>();
        break;
      case pucch_format::FORMAT_4:
        emplaced_pdu.format.emplace<ul_pucch_pdu_format_4>();
        break;
      case pucch_format::NOF_FORMATS:
      default:
        break;
    }

    return builder;
  }

  /// \brief Adds a PUCCH PDU to the message and returns a builder that helps to fill the parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder add_pucch_pdu(rnti_t rnti, pucch_format format_type)
  {
    ul_pucch_pdu_builder builder = add_pucch_pdu(format_type);
    builder.set_ue_specific_parameters(rnti);

    return builder;
  }

  /// \brief Adds a PUSCH PDU to the message and returns a builder that helps to fill the parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder add_pusch_pdu()
  {
    auto&                pdu = msg.pdus.emplace_back();
    ul_pusch_pdu_builder builder(pdu.pdu.emplace<ul_pusch_pdu>());

    return builder;
  }

  /// \brief Adds a PUSCH PDU to the message and returns a builder that helps to fill the parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.2 in table PUSCH PDU.
  ul_pusch_pdu_builder add_pusch_pdu(rnti_t rnti)
  {
    ul_pusch_pdu_builder builder = add_pusch_pdu();
    builder.set_ue_specific_parameters(rnti);

    return builder;
  }

  /// \brief Adds an SRS PDU to the message and returns a builder that helps to fill the parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table SRS PDU.
  ul_srs_pdu_builder add_srs_pdu()
  {
    auto&              pdu = msg.pdus.emplace_back();
    ul_srs_pdu_builder builder(pdu.pdu.emplace<ul_srs_pdu>());

    return builder;
  }
};

} // namespace fapi
} // namespace ocudu
