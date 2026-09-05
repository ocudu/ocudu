// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/nru/nru_message.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/enum_utils.h"

namespace ocudu {

/// NR-U PDU type field
///
/// Ref: TS 38.425 Sec. 5.5.3.1
enum class nru_pdu_type : uint8_t {
  dl_user_data            = 0, ///< DL user data (PDU type 0)
  dl_data_delivery_status = 1, ///< DL data delivery status (PDU type 1)
  assistance_information  = 2, ///< Assistance information (PDU type 2)
  reserved                = 3  ///< Reserved for future use (PDU type 3-15)
};

/// Convert unsigned integer to NR-U PDU type.
constexpr nru_pdu_type uint_to_nru_pdu_type(uint8_t num)
{
  return static_cast<nru_pdu_type>(num);
}

/// Packing and unpacking of NR user plane protocol (NR-U) messages
///
/// Ref: TS 38.425
class nru_packing
{
public:
  nru_packing(ocudulog::basic_logger& logger_) : logger(logger_) {}

  static nru_pdu_type get_pdu_type(byte_buffer_view container);

  bool unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const;

  bool unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const;

private:
  ocudulog::basic_logger& logger;
};

} // namespace ocudu

namespace fmt {

// NR-U PDU type field formatter
template <>
struct formatter<ocudu::nru_pdu_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::nru_pdu_type pdu_type, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"dl_user_data", "dl_data_delivery_status", "assistance_information"};
    if (to_underlying(pdu_type) < to_underlying(ocudu::nru_pdu_type::reserved)) {
      format_to(ctx.out(), "{}", options[to_underlying(pdu_type)]);
    }
    return format_to(ctx.out(), "reserved ({})", to_underlying(pdu_type));
  }
};
} // namespace fmt
