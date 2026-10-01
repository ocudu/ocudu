// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/base.h"
#include <cstdint>
#include <memory>
#include <string>

namespace ocudu {

/// Maximum supported size of a PDCP SDU
/// Ref: TS 38.323 Sec. 4.3.1
constexpr size_t pdcp_sdu_max_size = 9000;

/// Maximum supported size of a PDCP Control PDU
/// Ref: TS 38.323 Sec. 4.3.1
constexpr size_t pdcp_control_pdu_max_size = 9000;

/// PDCP Data/Control (D/C) field
/// Ref: TS 38.323 Sec. 6.3.7
enum class pdcp_dc_field : unsigned {
  control = 0b00, ///< Control PDU
  data    = 0b01  ///< Data PDU
};
constexpr unsigned to_number(pdcp_dc_field dc)
{
  return static_cast<unsigned>(dc);
}
inline const char* format_as(pdcp_dc_field dc)
{
  static constexpr const char* options[] = {"ctrl", "data"};
  return options[to_number(dc)];
}

/// \brief Reads the D/C field from the first (header) byte of a PDCP PDU
/// \param first_byte First byte of the PDU (passed by value)
/// \return Value of the D/C field
constexpr pdcp_dc_field pdcp_pdu_get_dc(uint8_t first_byte)
{
  return static_cast<pdcp_dc_field>((first_byte >> 7U) & 0x01U);
}

/// PDCP Control PDU type
/// Ref: TS 38.323 Sec. 6.3.8
enum class pdcp_control_pdu_type : unsigned {
  status_report              = 0b000, ///< PDCP status report.
  interspersed_rohc_feedback = 0b001, ///< Interspersed ROHC feedback.
  ehc_feedback               = 0b010, ///< EHC feedback.
  udc_feedback               = 0b011  ///< UDC feedback.
};
constexpr uint16_t to_number(pdcp_control_pdu_type type)
{
  return static_cast<uint16_t>(type);
}
inline const char* format_as(pdcp_control_pdu_type cpt)
{
  static constexpr const char* options[] = {"status_report", "rohc_feedback", "ehc_feedback", "udc_feedback"};
  auto                         idx       = to_number(cpt);
  return (idx < std::size(options)) ? options[idx] : "invalid";
}

/// \brief Reads the CPT field from the first (header) byte of a PDCP control PDU
/// \param first_byte First byte of the PDU (passed by value)
/// \return Value of the CPT field
constexpr pdcp_control_pdu_type pdcp_control_pdu_get_cpt(uint8_t first_byte)
{
  return static_cast<pdcp_control_pdu_type>((first_byte >> 4U) & 0x07U);
}

/// PDCP Data PDU header
/// Ref: TS 38.323 Sec. 6.2.2
struct pdcp_data_pdu_header {
  uint32_t sn; ///< Sequence number
};

/// PDCP Control PDU header
/// Ref: TS 38.323 Sec. 6.2.3
struct pdcp_control_pdu_header {
  pdcp_control_pdu_type cpt; ///< Control PDU type (control PDU only, ignored for data PDUs)
};

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::pdcp_data_pdu_header> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcp_data_pdu_header& hdr, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "sn={}", hdr.sn);
  }
};

template <>
struct formatter<ocudu::pdcp_control_pdu_header> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcp_control_pdu_header& hdr, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "cpt={}", hdr.cpt);
  }
};

} // namespace fmt
