// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gnb_constants.h"
#include "fmt/format.h"
#include <cstdint>
#include <type_traits>

namespace ocudu {

/// \brief Ue_index internally used to identify the UE CU-CP-wide.
enum class cu_cp_ue_index_t : uint64_t {
  min     = 0,
  max     = std::numeric_limits<uint64_t>::max() - 1,
  invalid = std::numeric_limits<uint64_t>::max()
};

/// Convert ue_index type to integer.
inline uint64_t cu_cp_ue_index_to_uint(cu_cp_ue_index_t index)
{
  return static_cast<uint64_t>(index);
}

/// Convert integer to ue_index type.
inline cu_cp_ue_index_t uint_to_ue_index(std::underlying_type_t<cu_cp_ue_index_t> index)
{
  return static_cast<cu_cp_ue_index_t>(index);
}

/// Maximum number of DUs supported by CU-CP (implementation-defined).
constexpr uint16_t CU_CP_MAX_NOF_DUS = 65535;

enum class cu_cp_du_index_t : uint16_t { min = 0, max = CU_CP_MAX_NOF_DUS - 1, invalid = CU_CP_MAX_NOF_DUS };

constexpr cu_cp_du_index_t uint_to_cu_cp_du_index(std::underlying_type_t<cu_cp_du_index_t> index)
{
  return static_cast<cu_cp_du_index_t>(index);
}

constexpr std::underlying_type_t<cu_cp_du_index_t> cu_cp_du_index_to_uint(cu_cp_du_index_t du_index)
{
  return static_cast<std::underlying_type_t<cu_cp_du_index_t>>(du_index);
}

/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
constexpr uint16_t CU_CP_MAX_NOF_CU_UPS = 65535;

enum class cu_cp_cu_up_index_t : uint16_t { min = 0, max = CU_CP_MAX_NOF_CU_UPS - 1, invalid = CU_CP_MAX_NOF_CU_UPS };

constexpr cu_cp_cu_up_index_t uint_to_cu_cp_cu_up_index(std::underlying_type_t<cu_cp_cu_up_index_t> index)
{
  return static_cast<cu_cp_cu_up_index_t>(index);
}

constexpr std::underlying_type_t<cu_cp_cu_up_index_t> cu_cp_cu_up_index_to_uint(cu_cp_cu_up_index_t cu_up_index)
{
  return static_cast<std::underlying_type_t<cu_cp_cu_up_index_t>>(cu_up_index);
}

/// Maximum number of AMFs supported by CU-CP (implementation-defined).
constexpr uint16_t CU_CP_MAX_NOF_AMFS = 65535;

enum class cu_cp_amf_index_t : uint16_t { min = 0, max = CU_CP_MAX_NOF_AMFS - 1, invalid = CU_CP_MAX_NOF_AMFS };

constexpr cu_cp_amf_index_t uint_to_cu_cp_amf_index(std::underlying_type_t<cu_cp_amf_index_t> index)
{
  return static_cast<cu_cp_amf_index_t>(index);
}

constexpr std::underlying_type_t<cu_cp_amf_index_t> cu_cp_amf_index_to_uint(cu_cp_amf_index_t amf_index)
{
  return static_cast<std::underlying_type_t<cu_cp_amf_index_t>>(amf_index);
}

/// Maximum number of cells per DU tracked by CU-CP (implementation-defined).
constexpr uint16_t CU_CP_MAX_NOF_DU_CELLS = MAX_CELLS_PER_DU;

enum class cu_cp_du_cell_index_t : uint16_t { min = 0, max = MAX_CELLS_PER_DU - 1, invalid = MAX_CELLS_PER_DU };

inline cu_cp_du_cell_index_t uint_to_cu_cp_du_cell_index(std::underlying_type_t<cu_cp_du_cell_index_t> index)
{
  return static_cast<cu_cp_du_cell_index_t>(index);
}

constexpr std::underlying_type_t<cu_cp_du_cell_index_t> cu_cp_du_cell_index_to_uint(cu_cp_du_cell_index_t cell_index)
{
  return static_cast<std::underlying_type_t<cu_cp_du_cell_index_t>>(cell_index);
}

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::cu_cp_ue_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::cu_cp_ue_index_t ue_idx, FormatContext& ctx) const
  {
    if (ue_idx == ocudu::cu_cp_ue_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", static_cast<uint64_t>(ue_idx));
  }
};

template <>
struct formatter<ocudu::cu_cp_du_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::cu_cp_du_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::cu_cp_du_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

template <>
struct formatter<ocudu::cu_cp_cu_up_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::cu_cp_cu_up_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::cu_cp_cu_up_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

template <>
struct formatter<ocudu::cu_cp_amf_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::cu_cp_amf_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::cu_cp_amf_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

template <>
struct formatter<ocudu::cu_cp_du_cell_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::cu_cp_du_cell_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::cu_cp_du_cell_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

} // namespace fmt
