// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/dl_pdcch_pdu.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"

namespace ocudu {
namespace fapi {

/// Uplink DCI PDU information.
struct ul_dci_pdu {
  dl_pdcch_pdu pdu;
};

/// Uplink DCI request message.
struct ul_dci_request {
  slot_point                                            slot;
  static_vector<ul_dci_pdu, MAX_UL_PDCCH_PDUS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::ul_dci_request> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_dci_request& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(), "UL_DCI.request slot={}", msg.slot);

    for (const auto& pdu : msg.pdus) {
      format_to(ctx.out(), "{}", pdu.pdu);
    }

    return ctx.out();
  }
};
} // namespace fmt
