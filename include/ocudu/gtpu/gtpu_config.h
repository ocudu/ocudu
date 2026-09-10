// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gtpu/gtpu_logical_interface.h"
#include "ocudu/ran/gtpu/gtpu_teid.h"
#include "ocudu/support/rate_limiting/token_bucket.h"
#include "fmt/base.h"
#include <chrono>
#include <string>

namespace ocudu {

/// Port specified for Encapsulated T-PDUs,
/// TS 29.281 Sec. 4.4.2.3
constexpr unsigned GTPU_PORT = 2152;

/// \brief Configurable parameters for GTP-U PSUP tunnels
struct gtpu_tunnel_psup_config {
  struct gtpu_tunnel_psup_rx_config {
    gtpu_logical_interface    lif = gtpu_logical_interface::invalid;
    gtpu_teid_t               local_teid;
    std::chrono::milliseconds t_reordering    = {};
    token_bucket*             ue_ambr_limiter = nullptr;
    bool                      warn_on_drop    = false;
    bool                      ignore_ue_ambr  = false;
    bool                      test_mode       = false;
  } rx;
  struct gtpu_tunnel_psup_tx_config {
    gtpu_logical_interface lif = gtpu_logical_interface::invalid;
    gtpu_teid_t            peer_teid;
    std::string            peer_addr;
    uint16_t               peer_port;
  } tx;
};

/// \brief Configurable parameters for GTP-U NR-U tunnels
struct gtpu_tunnel_nru_config {
  struct gtpu_tunnel_nru_rx_config {
    gtpu_logical_interface lif = gtpu_logical_interface::invalid;
    gtpu_teid_t            local_teid;
  } rx;
  struct gtpu_tunnel_nru_tx_config {
    gtpu_logical_interface lif = gtpu_logical_interface::invalid;
    gtpu_teid_t            peer_teid;
    std::string            peer_addr;
    uint16_t               peer_port;
  } tx;
};

} // namespace ocudu

//
// Formatters
//
namespace fmt {

// GTP-U PSUP RX config
template <>
struct formatter<ocudu::gtpu_tunnel_psup_config::gtpu_tunnel_psup_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_psup_config::gtpu_tunnel_psup_rx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "lif={} local_teid={} t_reordering={} warn_on_drop={} ignore_ue_ambr={}",
                     cfg.lif,
                     cfg.local_teid,
                     cfg.t_reordering,
                     cfg.warn_on_drop,
                     cfg.ignore_ue_ambr);
  }
};

// GTP-U PSUP TX config
template <>
struct formatter<ocudu::gtpu_tunnel_psup_config::gtpu_tunnel_psup_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_psup_config::gtpu_tunnel_psup_tx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "lif={} peer_teid={} peer_addr={} peer_port={}",
                     cfg.lif,
                     cfg.peer_teid,
                     cfg.peer_addr,
                     cfg.peer_port);
  }
};

// GTP-U PSUP config
template <>
struct formatter<ocudu::gtpu_tunnel_psup_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_psup_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "rx=[{}] tx=[{}]", cfg.rx, cfg.tx);
  }
};

// GTP-U NR-U RX config
template <>
struct formatter<ocudu::gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "lif={} local_teid={}", cfg.lif, cfg.local_teid);
  }
};

// GTP-U NR-U TX config
template <>
struct formatter<ocudu::gtpu_tunnel_nru_config::gtpu_tunnel_nru_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_nru_config::gtpu_tunnel_nru_tx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "lif={} peer_teid={} peer_addr={} peer_port={}",
                     cfg.lif,
                     cfg.peer_teid,
                     cfg.peer_addr,
                     cfg.peer_port);
  }
};

// GTP-U NR-U config
template <>
struct formatter<ocudu::gtpu_tunnel_nru_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::gtpu_tunnel_nru_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "rx=[{}] tx=[{}]", cfg.rx, cfg.tx);
  }
};

} // namespace fmt
