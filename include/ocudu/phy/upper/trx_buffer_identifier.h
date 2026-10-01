// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"
#include <cstdint>
#include <fmt/core.h>
#include <limits>

namespace ocudu {

/// Identifies transmit and receive buffers.
class trx_buffer_identifier
{
  /// Internal identifier type.
  using id_type = uint32_t;

  /// Invalid identifier.
  static constexpr id_type invalid_id = std::numeric_limits<id_type>::max();
  /// Unknown identifier.
  static constexpr id_type unknown_id = invalid_id - 1;

  /// Converts RNTI and HARQ identifiers to the internal identifier.
  static constexpr uint32_t to_id(rnti_t rnti, uint8_t harq)
  {
    return (static_cast<id_type>(rnti) << 8U) | static_cast<id_type>(harq);
  }

  /// Create from an internal identifier.
  constexpr trx_buffer_identifier(uint32_t id_) : id(id_) {}

public:
  /// Get an invalid buffer identifier.
  static constexpr trx_buffer_identifier invalid() { return invalid_id; }

  /// Get unknown buffer identifier.
  static constexpr trx_buffer_identifier unknown() { return unknown_id; }

  /// Default constructor - creates an invalid identifier.
  explicit trx_buffer_identifier() noexcept : id(invalid_id) {}

  /// Constructor from RNTI and HARQ process identifier.
  explicit constexpr trx_buffer_identifier(rnti_t rnti, uint8_t harq) noexcept : id(to_id(rnti, harq)) {}

  /// Equal comparison to other identifier.
  constexpr bool operator==(const trx_buffer_identifier& other) const { return other.id == id; }

  /// Not equal comparison with another identifier.
  constexpr bool operator!=(const trx_buffer_identifier& other) const { return !(*this == other); }

  /// Gets the RNTI.
  uint16_t get_rnti() const { return static_cast<uint16_t>(id >> 8); }

  /// Gets the HARQ process identifier.
  uint8_t get_harq() const { return static_cast<uint8_t>(id & 0xff); }

private:
  /// Internal identifier.
  id_type id;
};

} // namespace ocudu

namespace fmt {

/// Default formatter for trx_buffer_identifier.
template <>
struct formatter<ocudu::trx_buffer_identifier> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::trx_buffer_identifier& value, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "rnti={:#x} h_id={}", value.get_rnti(), value.get_harq());
  }
};

} // namespace fmt
