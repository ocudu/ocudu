// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcp/pdcp_discard_timer.h"
#include "ocudu/ran/pdcp/pdcp_rlc_mode.h"
#include "ocudu/ran/pdcp/pdcp_sn_size.h"
#include "ocudu/ran/pdcp/pdcp_t_reordering.h"
#include "ocudu/ran/rohc/rohc_config.h"
#include "ocudu/support/timers.h"
#include "fmt/format.h"
#include <cstdint>

namespace ocudu {

class pdcp_metrics_notifier;

/// PDCP NR SRB or DRB information.
enum class pdcp_rb_type { srb, drb };

/// \brief Returns the value range of the sequence numbers.
/// \param sn_size Length of the sequence number field in bits.
/// \return cardinality of sn_size.
constexpr uint32_t pcdp_sn_cardinality(uint16_t sn_size)
{
  ocudu_assert(sn_size < 32, "Cardinality of sn_size={} exceeds return type 'uint32_t'", sn_size);
  return (1 << sn_size);
}

/// \brief Returns the PDCP window size.
/// \param sn_size Length of the sequence number field in bits.
/// \return size of the window.
constexpr uint32_t pdcp_window_size(uint16_t sn_size)
{
  return pcdp_sn_cardinality(sn_size - 1);
}

/// \brief Returns the PDCP header size.
/// \param sn_size Length of the sequence number field in bits.
/// \return size of the data PDU header.
constexpr uint32_t pdcp_data_header_size(pdcp_sn_size sn_size)
{
  return sn_size == pdcp_sn_size::size12bits ? 2 : 3;
}

/// \brief Returns the PDCP trailer size.
/// \param rb_type           whether this is a SRB or DRB.
/// \param integrity_enabled whether integrity is enabled or not.
/// \return size of the data trailer.
constexpr uint32_t pdcp_data_trailer_size(pdcp_rb_type rb_type, bool integrity_enabled)
{
  constexpr uint32_t mac_i_size = 4;
  if (rb_type == pdcp_rb_type::srb) {
    return mac_i_size;
  }
  if (integrity_enabled) {
    return mac_i_size;
  }
  return 0;
}

/// Maximum supported PDCP SDU size, see TS 38.323, section 4.3.1.
constexpr uint16_t pdcp_max_sdu_size = 9000;

/// PDCP security direction.
enum class pdcp_security_direction { uplink, downlink };

/// Reordering timeout for serialization of TX PDUs after parallelized crypto operations.
constexpr uint32_t pdcp_tx_crypto_reordering_timeout_ms = 40;

/// The PDCP cannot re-use COUNTs, see TS 38.331, section 5.3.1.2.
/// To avoid this, we define two thresholds, one where we accept messages but notify the RRC and another where we no
/// longer accept messages. Here, we define some default values for this, both for TX and RX.
constexpr uint32_t pdcp_tx_default_max_count_notify = 0xc0000000;
constexpr uint32_t pdcp_tx_default_max_count_hard   = UINT32_MAX - 1;
constexpr uint32_t pdcp_rx_default_max_count_notify = 0xc0000000;
constexpr uint32_t pdcp_rx_default_max_count_hard   = 0xd0000000;
struct pdcp_max_count {
  uint32_t notify;
  uint32_t hard;
};

struct pdcp_custom_config_base {
  pdcp_max_count max_count = {pdcp_tx_default_max_count_notify, pdcp_tx_default_max_count_hard};
};

struct pdcp_custom_config_tx : public pdcp_custom_config_base {
  bool warn_on_drop = false;
  bool test_mode    = false;
};

struct pdcp_custom_config_rx : public pdcp_custom_config_base {
  bool warn_on_drop = false;
};

/// \brief Non-standard configurable parameters for PDCP.
///
/// Configurable parameters for the PDCP entity that are not explicitly specified in the RRC specifications. This
/// separation is necessary, as the CU-CP cannot transmit these parameters to the CU-UP, so it's necessary for the CU-UP
/// to store these configurations itself.
struct pdcp_custom_config {
  timer_duration         metrics_period;
  pdcp_metrics_notifier* metrics_notifier = nullptr;
  pdcp_custom_config_tx  tx               = {};
  pdcp_custom_config_rx  rx               = {};
};

/// \brief Configurable parameters for PDCP that are common for both TX and RX.
/// Ref: 3GPP TS 38.331 version 15.2.1.
struct pdcp_config_common {
  pdcp_rb_type               rb_type;
  pdcp_rlc_mode              rlc_mode;
  pdcp_sn_size               sn_size;
  pdcp_security_direction    direction;
  std::optional<rohc_config> header_compression;
};

struct pdcp_tx_config : pdcp_config_common {
  std::optional<pdcp_discard_timer> discard_timer;
  bool                              status_report_required;
  pdcp_custom_config_tx             custom;
};

struct pdcp_rx_config : pdcp_config_common {
  bool                  out_of_order_delivery;
  pdcp_t_reordering     t_reordering;
  pdcp_custom_config_rx custom;
};

/// \brief Configurable parameters for PDCP.
///
/// Parameters and valid values for them are taken from the RRC-NR PDCP-Config Information Element.
/// Ref: 3GPP TS 38.331 version 15.2.1.
struct pdcp_config {
  pdcp_rb_type               rb_type;
  pdcp_rlc_mode              rlc_mode;
  std::optional<rohc_config> header_compression;
  bool                       integrity_protection_required;
  bool                       ciphering_required;
  struct {
    pdcp_sn_size                      sn_size;
    pdcp_security_direction           direction;
    std::optional<pdcp_discard_timer> discard_timer;
    bool                              status_report_required;
  } tx;
  struct {
    pdcp_sn_size            sn_size;
    pdcp_security_direction direction;
    bool                    out_of_order_delivery;
    pdcp_t_reordering       t_reordering;
  } rx;
  pdcp_custom_config custom;

  pdcp_tx_config get_tx_config() const
  {
    pdcp_tx_config cfg;
    cfg.rb_type                = rb_type;
    cfg.rlc_mode               = rlc_mode;
    cfg.sn_size                = tx.sn_size;
    cfg.direction              = tx.direction;
    cfg.header_compression     = header_compression;
    cfg.discard_timer          = tx.discard_timer;
    cfg.status_report_required = tx.status_report_required;
    cfg.custom                 = custom.tx;
    return cfg;
  }
  pdcp_rx_config get_rx_config() const
  {
    pdcp_rx_config cfg;
    cfg.rb_type               = rb_type;
    cfg.rlc_mode              = rlc_mode;
    cfg.sn_size               = rx.sn_size;
    cfg.direction             = rx.direction;
    cfg.header_compression    = header_compression;
    cfg.out_of_order_delivery = rx.out_of_order_delivery;
    cfg.t_reordering          = rx.t_reordering;
    cfg.custom                = custom.rx;
    return cfg;
  }
};

/// \brief Make default SRB parameters for PDCP.
/// Ref: 3GPP TS 38.331, section 9.2.1.
inline pdcp_config pdcp_make_default_srb_config()
{
  pdcp_config config = {};
  // Common TX/RX parameters.
  config.rb_type                       = pdcp_rb_type::srb;
  config.rlc_mode                      = pdcp_rlc_mode::am;
  config.header_compression            = std::nullopt;
  config.integrity_protection_required = true;
  config.ciphering_required            = true;

  // Tx config.
  config.tx.sn_size                = pdcp_sn_size::size12bits;
  config.tx.direction              = pdcp_security_direction::downlink;
  config.tx.status_report_required = false;
  config.tx.discard_timer          = pdcp_discard_timer::infinity;

  // Rx config.
  config.rx.sn_size               = pdcp_sn_size::size12bits;
  config.rx.direction             = pdcp_security_direction::uplink;
  config.rx.out_of_order_delivery = false;
  config.rx.t_reordering          = pdcp_t_reordering::infinity;

  // Custom config.
  config.custom = {};

  return config;
}

struct pdcp_count_info {
  uint32_t sn  = 0;
  uint32_t hfn = 0;
};

struct pdcp_sn_status_info {
  pdcp_count_info ul_count;
  pdcp_count_info dl_count;
};

} // namespace ocudu

//
// Formatters
//
namespace fmt {

template <>
struct formatter<ocudu::pdcp_rb_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_rb_type type, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"SRB", "DRB"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(type)]);
  }
};

template <>
struct formatter<ocudu::pdcp_custom_config_tx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_custom_config_tx cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "count_notify={} count_max={} warn_on_drop={} test_mode={}",
                     cfg.max_count.notify,
                     cfg.max_count.hard,
                     cfg.warn_on_drop,
                     cfg.test_mode);
  }
};

template <>
struct formatter<ocudu::pdcp_custom_config_rx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_custom_config_rx cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "count_notify={} count_max={} warn_on_drop={}",
                     cfg.max_count.notify,
                     cfg.max_count.hard,
                     cfg.warn_on_drop);
  }
};

template <>
struct formatter<ocudu::pdcp_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcp_tx_config& cfg, FormatContext& ctx) const
  {
    auto out = ctx.out();
    out      = format_to(out,
                    "rb_type={} rlc_mode={} sn_size={} discard_timer={}",
                    cfg.rb_type,
                    cfg.rlc_mode,
                    cfg.sn_size,
                    cfg.discard_timer);
    if (cfg.header_compression.has_value()) {
      out = format_to(out, " {}", *cfg.header_compression);
    }
    out = format_to(out, " {}", cfg.custom);
    return out;
  }
};

template <>
struct formatter<ocudu::pdcp_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcp_rx_config& cfg, FormatContext& ctx) const
  {
    auto out = ctx.out();
    out      = format_to(ctx.out(),
                    "rb_type={} rlc_mode={} sn_size={} t_reordering={}",
                    cfg.rb_type,
                    cfg.rlc_mode,
                    cfg.sn_size,
                    cfg.t_reordering);
    if (cfg.header_compression.has_value()) {
      out = format_to(out, " {}", *cfg.header_compression);
    }
    out = format_to(out, " {}", cfg.custom);
    return out;
  }
};

template <>
struct formatter<ocudu::pdcp_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcp_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} {} int_req={} cip_req={} TX=[sn_size={} discard_timer={}] "
                     "RX=[sn_size={} t_reordering={} out_of_order={}] custom_tx=[{}] custom_rx=[{}]",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.header_compression,
                     cfg.integrity_protection_required,
                     cfg.ciphering_required,
                     cfg.tx.sn_size,
                     cfg.tx.discard_timer,
                     cfg.rx.sn_size,
                     cfg.rx.t_reordering,
                     cfg.rx.out_of_order_delivery,
                     cfg.custom.tx,
                     cfg.custom.rx);
  }
};

} // namespace fmt
