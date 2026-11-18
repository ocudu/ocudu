// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/ul_prach_pdu.h"
#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"
#include "ocudu/fapi/p7/messages/ul_pusch_pdu.h"
#include "ocudu/fapi/p7/messages/ul_srs_pdu.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {
namespace fapi {

/// Uplink TTI request message.
struct ul_tti_request {
  slot_point                                                slot;
  static_vector<ul_prach_pdu, MAX_PRACH_OCCASIONS_PER_SLOT> prach_pdus;
  static_vector<ul_pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT>      pusch_pdus;
  static_vector<ul_pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT>      pucch_pdus;
  static_vector<ul_srs_pdu, MAX_SRS_PDUS_PER_SLOT>          srs_pdus;

  /// Returns true if the message has no PDUs.
  bool empty() const { return prach_pdus.empty() && pusch_pdus.empty() && pucch_pdus.empty() && srs_pdus.empty(); }
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::ul_tti_request> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_tti_request& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(), "UL_TTI.request slot={}", msg.slot);

    for (const auto& pdu : msg.prach_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.pusch_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.pucch_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }
    for (const auto& pdu : msg.srs_pdus) {
      format_to(ctx.out(), "{}", pdu);
    }

    return ctx.out();
  }
};
} // namespace fmt
