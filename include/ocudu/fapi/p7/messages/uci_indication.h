// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "formatter/formatter_helpers.h"
#include "ocudu/fapi/p7/messages/uci_pucch_pdu_format_0_1.h"
#include "ocudu/fapi/p7/messages/uci_pucch_pdu_format_2_3_4.h"
#include "ocudu/fapi/p7/messages/uci_pusch_pdu.h"
#include "ocudu/ran/slot_point.h"
#include <variant>

namespace ocudu {
namespace fapi {

/// UCI indication message.
struct uci_indication {
  /// UCI indication PDU format.
  using uci_indication_pdu =
      std::variant<std::monostate, uci_pusch_pdu, uci_pucch_pdu_format_0_1, uci_pucch_pdu_format_2_3_4>;

  slot_point         slot;
  uci_indication_pdu pdu;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::uci_indication> {
private:
  /// Logs the UCI PUSCH PDU.
  template <typename FormatContext>
  static auto
  log_uci_pusch_pdu(FormatContext& ctx, const ocudu::fapi::uci_pusch_pdu& pdu, ocudu::subcarrier_spacing scs)
  {
    format_to(ctx.out(), " PUSCH rnti={}", pdu.rnti);

    if (pdu.ul_sinr_metric_dB.has_value()) {
      format_to(ctx.out(), " sinr={:.1f}dB", *pdu.ul_sinr_metric_dB);
    }

    ocudu::fapi::append_time_advance(ctx, pdu.timing_advance_offset, scs);

    if (pdu.rssi.has_value()) {
      format_to(ctx.out(), " rssi={}", *pdu.rssi);
    }

    if (pdu.rsrp.has_value()) {
      format_to(ctx.out(), " rsrp={}", *pdu.rsrp);
    }

    if (pdu.harq.has_value()) {
      format_to(ctx.out(),
                " HARQ: detection={} bit_len={}",
                underlying(pdu.harq->detection_status),
                pdu.harq->expected_bit_length);
    }

    if (pdu.csi_part1.has_value()) {
      format_to(ctx.out(),
                " CSI1: detection={} bit_len={}",
                underlying(pdu.csi_part1->detection_status),
                pdu.csi_part1->expected_bit_length);
    }

    if (pdu.csi_part2.has_value()) {
      format_to(ctx.out(),
                " CSI2: detection={} bit_len={}",
                underlying(pdu.csi_part2->detection_status),
                pdu.csi_part2->expected_bit_length);
    }

    return ctx.out();
  }

  /// Logs the UCI PUCCH Format 0 or 1 PDU.
  template <typename FormatContext>
  static auto log_uci_pucch_f0_f1_pdu(FormatContext&                               ctx,
                                      const ocudu::fapi::uci_pucch_pdu_format_0_1& pdu,
                                      ocudu::subcarrier_spacing                    scs)
  {
    format_to(ctx.out(), " PUCCH format 0/1 format={} rnti={}", underlying(pdu.pucch_format), pdu.rnti);

    if (pdu.ul_sinr_metric_dB.has_value()) {
      format_to(ctx.out(), " sinr={:.1f}dB", *pdu.ul_sinr_metric_dB);
    }

    ocudu::fapi::append_time_advance(ctx, pdu.timing_advance_offset, scs);

    if (pdu.rssi.has_value()) {
      format_to(ctx.out(), " rssi={}", *pdu.rssi);
    }

    if (pdu.rsrp.has_value()) {
      format_to(ctx.out(), " rsrp={}", *pdu.rsrp);
    }

    if (pdu.sr.has_value()) {
      format_to(ctx.out(), " SR: sr={}", pdu.sr->sr_detected ? "detected" : "not detected");
    }

    if (pdu.harq.has_value()) {
      format_to(ctx.out(), " HARQ: harq_ack=");
      for (unsigned i = 0, e = pdu.harq->harq_values.size(), last = e - 1; i != e; ++i) {
        format_to(ctx.out(), "{}", to_string(pdu.harq->harq_values[i]));
        if (i != last) {
          format_to(ctx.out(), ",");
        }
      }
    }

    return ctx.out();
  }

  /// Logs the UCI PUCCH Format 2, 3 or 4 PDU.
  template <typename FormatContext>
  static auto log_uci_pucch_f234_pdu(FormatContext&                                 ctx,
                                     const ocudu::fapi::uci_pucch_pdu_format_2_3_4& pdu,
                                     ocudu::subcarrier_spacing                      scs)
  {
    format_to(ctx.out(), " PUCCH format 2/3/4 format={} rnti={}", underlying(pdu.pucch_format) + 2, pdu.rnti);

    if (pdu.ul_sinr_metric_dB.has_value()) {
      format_to(ctx.out(), " sinr={:.1f}dB", *pdu.ul_sinr_metric_dB);
    }

    ocudu::fapi::append_time_advance(ctx, pdu.timing_advance_offset, scs);

    if (pdu.rssi.has_value()) {
      format_to(ctx.out(), " rssi={}", *pdu.rssi);
    }

    if (pdu.rsrp.has_value()) {
      format_to(ctx.out(), " rsrp={}", *pdu.rsrp);
    }

    if (pdu.sr.has_value()) {
      format_to(ctx.out(), " SR: bit_len={}", pdu.sr->sr_payload.size());
    }

    if (pdu.harq.has_value()) {
      format_to(ctx.out(),
                " HARQ: detection={} bit_len={}",
                underlying(pdu.harq->detection_status),
                pdu.harq->expected_bit_length);
    }

    if (pdu.csi_part1.has_value()) {
      format_to(ctx.out(),
                " CSI1: detection={} bit_len={}",
                underlying(pdu.csi_part1->detection_status),
                pdu.csi_part1->expected_bit_length);
    }

    if (pdu.csi_part2.has_value()) {
      format_to(ctx.out(),
                " CSI2: detection={} bit_len={}",
                underlying(pdu.csi_part2->detection_status),
                pdu.csi_part2->expected_bit_length);
    }

    return ctx.out();
  }

public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::uci_indication& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(), "UCI.indication slot={}", msg.slot);

    if (const auto* uci_pusch = std::get_if<ocudu::fapi::uci_pusch_pdu>(&msg.pdu)) {
      log_uci_pusch_pdu(ctx, *uci_pusch, msg.slot.scs());
    } else if (const auto* uci_pusch_format_0_1 = std::get_if<ocudu::fapi::uci_pucch_pdu_format_0_1>(&msg.pdu)) {
      log_uci_pucch_f0_f1_pdu(ctx, *uci_pusch_format_0_1, msg.slot.scs());
    } else if (const auto* uci_pusch_format_2_3_4 = std::get_if<ocudu::fapi::uci_pucch_pdu_format_2_3_4>(&msg.pdu)) {
      log_uci_pucch_f234_pdu(ctx, *uci_pusch_format_2_3_4, msg.slot.scs());
    }

    return ctx.out();
  }
};
} // namespace fmt
