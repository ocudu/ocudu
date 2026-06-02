// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/tai.h"
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

/// Maximum number of XN-C peers supported by CU-CP (implementation-defined).
constexpr uint16_t MAX_NOF_XNC_PEERS    = 65535;
constexpr uint16_t MAX_NOF_XNC_GATEWAYS = 256;

enum class xnc_peer_index_t : uint16_t { min = 0, max = MAX_NOF_XNC_PEERS - 1, invalid = MAX_NOF_XNC_PEERS };

/// Convert integer to XN-C peer index type.
constexpr xnc_peer_index_t uint_to_xnc_peer_index(std::underlying_type_t<xnc_peer_index_t> index)
{
  return static_cast<xnc_peer_index_t>(index);
}

/// XN-C gateway index in `cu_cp_configuration::xnap_params::xnc_gws`.
enum class xnc_gateway_index_t : uint16_t { min = 0, max = MAX_NOF_XNC_GATEWAYS - 1, invalid = MAX_NOF_XNC_GATEWAYS };

/// Convert integer to XN-C gateway index type.
constexpr xnc_gateway_index_t uint_to_xnc_gateway_index(std::underlying_type_t<xnc_gateway_index_t> index)
{
  return static_cast<xnc_gateway_index_t>(index);
}

/// Convert XN-C gateway index type to integer.
constexpr std::underlying_type_t<xnc_gateway_index_t> xnc_gateway_index_to_uint(xnc_gateway_index_t gw_index)
{
  return static_cast<std::underlying_type_t<xnc_gateway_index_t>>(gw_index);
}

struct cu_cp_global_gnb_id {
  plmn_identity plmn_id = plmn_identity::test_value();
  gnb_id_t      gnb_id;
};

/// Notification from the E1AP/F1AP that transaction reference information for some UEs has been lost.
struct ue_transaction_info_loss_event {
  std::vector<cu_cp_ue_index_t> ues_lost;
};

/// Common interface reset message for E1AP (E1 Reset), F1AP (F1 Reset) and NGAP (NG Reset).
struct cu_cp_reset {
  std::variant<e1ap_cause_t, f1ap_cause_t, ngap_cause_t> cause;
  bool                                                   interface_reset = false;
  std::vector<cu_cp_ue_index_t>                          ues_to_reset;
};

struct cu_cp_user_location_info_nr {
  nr_cell_global_id_t     nr_cgi;
  tai_t                   tai;
  std::optional<uint64_t> time_stamp;
};

struct cu_cp_initial_ue_message {
  cu_cp_ue_index_t               ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                    nas_pdu;
  establishment_cause_t          establishment_cause;
  cu_cp_user_location_info_nr    user_location_info;
  std::optional<five_g_s_tmsi_t> five_g_s_tmsi;
  std::optional<uint16_t>        amf_set_id;
};

struct cu_cp_ul_nas_transport {
  cu_cp_ue_index_t            ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                 nas_pdu;
  cu_cp_user_location_info_nr user_location_info;
};

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

/// XNC peer index formatter.
template <>
struct formatter<ocudu::xnc_peer_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::xnc_peer_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::xnc_peer_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

/// XNC gateway index formatter.
template <>
struct formatter<ocudu::xnc_gateway_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::xnc_gateway_index_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::xnc_gateway_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

} // namespace fmt
