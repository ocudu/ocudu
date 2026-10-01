// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/shared_transport_block.h"

namespace ocudu {
namespace fapi {

/// Transmission data request PDU information.
struct tx_data_req_pdu {
  tx_data_req_pdu() = default;

  tx_data_req_pdu(uint16_t pdu_index_, uint8_t cw_index_, shared_transport_block pdu_) :
    pdu_index(pdu_index_), cw_index(cw_index_), pdu(std::move(pdu_))
  {
  }

  uint16_t               pdu_index;
  uint8_t                cw_index;
  shared_transport_block pdu;
};

/// Transmission request message.
struct tx_data_request {
  slot_point                                              slot;
  static_vector<tx_data_req_pdu, MAX_PDSCH_PDUS_PER_SLOT> pdus;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::tx_data_request> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::tx_data_request& msg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "Tx_Data.request slot={} nof_pdus={}", msg.slot, msg.pdus.size());
  }
};
} // namespace fmt
