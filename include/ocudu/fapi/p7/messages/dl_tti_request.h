// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/dl_csi_rs_pdu.h"
#include "ocudu/fapi/p7/messages/dl_pdcch_pdu.h"
#include "ocudu/fapi/p7/messages/dl_pdsch_pdu.h"
#include "ocudu/fapi/p7/messages/dl_prs_pdu.h"
#include "ocudu/fapi/p7/messages/dl_ssb_pdu.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"

namespace ocudu {
namespace fapi {

/// Downlink TTI request message.
struct dl_tti_request {
  slot_point                                              slot;
  static_vector<dl_pdcch_pdu, MAX_DL_PDCCH_PDUS_PER_SLOT> pdcch_pdus;
  static_vector<dl_pdsch_pdu, MAX_PDSCH_PDUS_PER_SLOT>    pdsch_pdus;
  static_vector<dl_ssb_pdu, MAX_SSB_PER_SLOT>             ssb_pdus;
  static_vector<dl_csi_rs_pdu, MAX_CSI_RS_PDUS_PER_SLOT>  csi_rs_pdus;
  static_vector<dl_prs_pdu, MAX_PRS_PDUS_PER_SLOT>        prs_pdus;

  /// Returns true if the message has no PDUs.
  bool empty() const
  {
    return pdcch_pdus.empty() && pdsch_pdus.empty() && ssb_pdus.empty() && csi_rs_pdus.empty() && prs_pdus.empty();
  }
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::dl_tti_request> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::dl_tti_request& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(), "DL_TTI.request slot={}", msg.slot);

    for (const auto& pdu : msg.pdcch_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.ssb_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.csi_rs_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.prs_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.pdsch_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }

    return ctx.out();
  }
};
} // namespace fmt
