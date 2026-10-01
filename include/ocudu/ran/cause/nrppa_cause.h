// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/base.h"
#include <variant>

namespace ocudu {

/// The NRPPa radio network cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_radio_network_t : uint8_t {
  unspecified = 0,
  requested_item_not_supported,
  requested_item_temporarily_not_available
};

/// The NRPPa protocol cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  unspecified,
  abstract_syntax_error_falsely_constructed_msg
};

/// The NRPPa misc cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_misc_t : uint8_t { unspecified = 0 };

/// The NRPPa cause to indicate the reason for a particular event, see TS 38.455 section 9.2.1.
/// The NRPPa cause is a union of the radio network cause, protocol cause and misc cause.
using nrppa_cause_t = std::variant<nrppa_cause_radio_network_t, nrppa_cause_protocol_t, nrppa_cause_misc_t>;

} // namespace ocudu

namespace fmt {

// nrppa_cause_t formatter
template <>
struct formatter<ocudu::nrppa_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::nrppa_cause_t o, FormatContext& ctx)
  {
    if (const auto* result = std::get_if<ocudu::nrppa_cause_radio_network_t>(&o)) {
      return format_to(ctx.out(), "radio_network-id{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::nrppa_cause_protocol_t>(&o)) {
      return format_to(ctx.out(), "protocol-id{}", *result);
    }
    return format_to(ctx.out(), "misc-id{}", std::get<ocudu::nrppa_cause_misc_t>(o));
  }
};

} // namespace fmt
