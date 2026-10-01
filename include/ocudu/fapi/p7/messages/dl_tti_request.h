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

/// Common downlink PDU information.
struct dl_tti_request_pdu {
  std::variant<dl_pdcch_pdu, dl_pdsch_pdu, dl_csi_rs_pdu, dl_ssb_pdu, dl_prs_pdu> pdu;
};

/// Downlink TTI request message.
struct dl_tti_request {
  slot_point                                              slot;
  static_vector<dl_tti_request_pdu, MAX_DL_PDUS_PER_SLOT> pdus;
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

    for (const auto& pdu : msg.pdus) {
      if (const auto* csi_rs_pdu = std::get_if<ocudu::fapi::dl_csi_rs_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *csi_rs_pdu);
        continue;
      }

      if (const auto* pdcch_pdu = std::get_if<ocudu::fapi::dl_pdcch_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *pdcch_pdu);
        continue;
      }

      if (const auto* pdsch_pdu = std::get_if<ocudu::fapi::dl_pdsch_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *pdsch_pdu);
        continue;
      }

      if (const auto* ssb_pdu = std::get_if<ocudu::fapi::dl_ssb_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *ssb_pdu);
        continue;
      }

      if (const auto* prs_pdu = std::get_if<ocudu::fapi::dl_prs_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *prs_pdu);
        continue;
      }
    }

    return ctx.out();
  }
};
} // namespace fmt
