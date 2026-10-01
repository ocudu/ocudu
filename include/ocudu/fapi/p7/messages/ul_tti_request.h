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

/// Common uplink PDU information.
struct ul_tti_request_pdu {
  std::variant<ul_prach_pdu, ul_pusch_pdu, ul_pucch_pdu, ul_srs_pdu> pdu;
};

/// Uplink TTI request message.
struct ul_tti_request {
  slot_point                                              slot;
  static_vector<ul_tti_request_pdu, MAX_UL_PDUS_PER_SLOT> pdus;
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

    for (const auto& pdu : msg.pdus) {
      if (const auto* prach_pdu = std::get_if<ocudu::fapi::ul_prach_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *prach_pdu);
        continue;
      }

      if (const auto* pucch_pdu = std::get_if<ocudu::fapi::ul_pucch_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *pucch_pdu);
        continue;
      }

      if (const auto* pusch_pdu = std::get_if<ocudu::fapi::ul_pusch_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *pusch_pdu);
        continue;
      }

      if (const auto* srs_pdu = std::get_if<ocudu::fapi::ul_srs_pdu>(&pdu.pdu)) {
        format_to(ctx.out(), "{}", *srs_pdu);
        continue;
      }

      ocudu_assert(0, "UL_TTI.request PDU type value not recognized.");
    }

    return ctx.out();
  }
};
} // namespace fmt
