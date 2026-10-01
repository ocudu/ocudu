// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/dl_csi_rs_pdu_builder.h"
#include "ocudu/fapi/p7/builders/dl_pdcch_pdu_builder.h"
#include "ocudu/fapi/p7/builders/dl_pdsch_pdu_builder.h"
#include "ocudu/fapi/p7/builders/dl_prs_pdu_builder.h"
#include "ocudu/fapi/p7/builders/dl_ssb_pdu_builder.h"
#include "ocudu/fapi/p7/messages/dl_tti_request.h"

namespace ocudu {
namespace fapi {

class dl_tti_request_builder
{
public:
  /// Constructs a builder that will help to fill the given DL TTI request message.
  explicit dl_tti_request_builder(dl_tti_request& msg_) : msg(msg_) {}

  /// \brief Sets the DL_TTI.request slot point and returns a reference to the builder.
  ///
  /// nPDUs and nPDUsOfEachType properties are filled by the add_*_pdu() functions.
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2 in table DL_TTI.request message body.
  dl_tti_request_builder& set_slot(slot_point slot)
  {
    msg.slot = slot;

    return *this;
  }

  /// Adds a PDCCH PDU to the message, fills its basic parameters using the given arguments and returns a PDCCH PDU
  /// builder.
  dl_pdcch_pdu_builder add_pdcch_pdu()
  {
    // Add a new pdu.
    dl_tti_request_pdu&  pdu = msg.pdus.emplace_back();
    dl_pdcch_pdu_builder builder(pdu.pdu.emplace<dl_pdcch_pdu>());

    return builder;
  }

  /// Adds a PDSCH PDU to the message, fills its basic parameters using the given arguments and returns a PDSCH PDU
  /// builder.
  dl_pdsch_pdu_builder add_pdsch_pdu(rnti_t rnti)
  {
    dl_pdsch_pdu_builder builder = add_pdsch_pdu();
    builder.set_ue_specific_parameters(rnti);

    return builder;
  }

  /// Adds a PDSCH PDU to the message, fills its basic parameters using the given arguments and returns a PDSCH PDU
  /// builder.
  dl_pdsch_pdu_builder add_pdsch_pdu()
  {
    // Add a new PDU.
    dl_tti_request_pdu&  pdu = msg.pdus.emplace_back();
    dl_pdsch_pdu_builder builder(pdu.pdu.emplace<dl_pdsch_pdu>());

    return builder;
  }

  /// Adds a CSI-RS PDU to the message and returns a CSI-RS PDU builder.
  dl_csi_rs_pdu_builder add_csi_rs_pdu()
  {
    // Add a new PDU.
    dl_tti_request_pdu&   pdu = msg.pdus.emplace_back();
    dl_csi_rs_pdu_builder builder(pdu.pdu.emplace<dl_csi_rs_pdu>());

    return builder;
  }

  /// Adds a SSB PDU to the message and returns a SSB PDU builder.
  dl_ssb_pdu_builder add_ssb_pdu()
  {
    // Add a new PDU.
    dl_tti_request_pdu& pdu = msg.pdus.emplace_back();
    dl_ssb_pdu_builder  builder(pdu.pdu.emplace<dl_ssb_pdu>());

    return builder;
  }

  /// Adds a PRS PDU to the message and returns a PRS PDU builder.
  dl_prs_pdu_builder add_prs_pdu()
  {
    // Add a new PDU.
    dl_tti_request_pdu& pdu = msg.pdus.emplace_back();
    dl_prs_pdu_builder  builder(pdu.pdu.emplace<dl_prs_pdu>());

    return builder;
  }

private:
  dl_tti_request& msg;
};

} // namespace fapi
} // namespace ocudu
