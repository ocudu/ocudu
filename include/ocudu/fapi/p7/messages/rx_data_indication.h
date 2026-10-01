// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// Reception data indication PDU information.
struct rx_data_indication_pdu {
  uint32_t               handle = 0;
  rnti_t                 rnti;
  std::optional<uint8_t> rapid;
  harq_id_t              harq_id;
  span<const uint8_t>    transport_block;
};

/// Reception data indication message.
struct rx_data_indication {
  slot_point             slot;
  rx_data_indication_pdu pdu;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::rx_data_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::rx_data_indication& msg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "Rx_Data.indication slot={} rnti={} harq_id={} tbs={}",
                     msg.slot,
                     msg.pdu.rnti,
                     msg.pdu.harq_id,
                     msg.pdu.transport_block.size());
  }
};
} // namespace fmt
