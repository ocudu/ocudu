// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include "fmt/base.h"
#include <cstdint>

namespace ocudu {

/// See TS 38.463 Section 9.3.1.24: QoS Flow ID valid values: (0..63)
constexpr uint8_t MAX_NOF_QOS_FLOWS = 64;

/// \brief QoS Flow ID.
/// \remark See TS 38.463 Section 9.3.1.21: QoS Flow ID valid values: (0..63)
enum class qos_flow_id_t : uint8_t { min = 0, max = MAX_NOF_QOS_FLOWS - 1, invalid = MAX_NOF_QOS_FLOWS };

/// Convert integer to QoS Flow ID type.
constexpr qos_flow_id_t uint_to_qos_flow_id(uint8_t idx)
{
  return static_cast<qos_flow_id_t>(idx);
}

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::qos_flow_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::qos_flow_id_t& qfi, FormatContext& ctx) const
  {
    switch (qfi) {
      case ocudu::qos_flow_id_t::invalid:
        return format_to(ctx.out(), "invalid QFI");
      default:
        return format_to(ctx.out(), "QFI={:#}", to_underlying(qfi));
    }
  }
};

} // namespace fmt
