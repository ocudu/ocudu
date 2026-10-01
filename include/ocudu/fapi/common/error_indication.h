// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/common/error_code.h"
#include "ocudu/ran/slot_point.h"
#include "fmt/format.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// Message type IDs.
enum class message_type_id : uint16_t {
  param_request      = 0x00,
  param_response     = 0x01,
  config_request     = 0x02,
  config_response    = 0x03,
  start_request      = 0x04,
  stop_request       = 0x05,
  stop_indication    = 0x06,
  error_indication   = 0x07,
  dl_tti_request     = 0x80,
  dl_tti_response    = 0x8a,
  ul_tti_request     = 0x81,
  slot_indication    = 0x82,
  ul_dci_request     = 0x83,
  tx_data_request    = 0x84,
  rx_data_indication = 0x85,
  crc_indication     = 0x86,
  uci_indication     = 0x87,
  srs_indication     = 0x88,
  rach_indication    = 0x89
};

/// Encodes the error indication message.
struct error_indication {
  std::optional<slot_point> slot;
  message_type_id           message_id;
  error_code_id             error_code;
  std::optional<slot_point> expected_slot;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::error_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::error_indication& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "Error.indication slot={} error_code={} msg_id={}",
              *msg.slot,
              underlying(msg.error_code),
              underlying(msg.message_id));

    if (msg.error_code == ocudu::fapi::error_code_id::out_of_sync && msg.expected_slot) {
      format_to(ctx.out(), " expected_slot={}", *msg.expected_slot);
    }

    return ctx.out();
  }
};
} // namespace fmt
