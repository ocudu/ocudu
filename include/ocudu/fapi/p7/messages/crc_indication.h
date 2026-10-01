// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "formatter/formatter_helpers.h"
#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// Reception data indication PDU information.
struct crc_ind_pdu {
  uint32_t                       handle = 0;
  rnti_t                         rnti;
  std::optional<uint8_t>         rapid;
  harq_id_t                      harq_id;
  bool                           tb_crc_status_ok;
  std::optional<float>           ul_sinr_metric_dB;
  std::optional<phy_time_unit>   timing_advance_offset;
  std::optional<fapi_power_unit> rssi;
  std::optional<fapi_power_unit> rsrp;
};

/// CRC indication message.
struct crc_indication {
  slot_point  slot;
  crc_ind_pdu pdu;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::crc_indication> {
private:
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::crc_indication& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "CRC.indication slot={} rnti={} harq_id={} tb_status={}",
              msg.slot,
              msg.pdu.rnti,
              msg.pdu.harq_id,
              msg.pdu.tb_crc_status_ok ? "OK" : "KO");

    ocudu::fapi::append_time_advance(ctx, msg.pdu.timing_advance_offset, msg.slot.scs());

    if (msg.pdu.ul_sinr_metric_dB.has_value()) {
      format_to(ctx.out(), " sinr={:.1f}dB", *msg.pdu.ul_sinr_metric_dB);
    }
    if (msg.pdu.rssi.has_value()) {
      format_to(ctx.out(), " rssi={}", *msg.pdu.rssi);
    }
    if (msg.pdu.rsrp.has_value()) {
      format_to(ctx.out(), " rsrp={}", *msg.pdu.rsrp);
    }

    return ctx.out();
  }
};
} // namespace fmt
