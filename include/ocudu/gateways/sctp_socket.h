// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/io/unique_fd.h"
#include "fmt/std.h"
#include <chrono>
#include <cstdint>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/socket.h>

namespace ocudu {

struct sctp_socket_params {
  /// Name of the interface for logging purposes.
  std::string                              if_name;
  int                                      ai_family;
  int                                      ai_socktype;
  bool                                     reuse_addr        = false;
  bool                                     non_blocking_mode = false;
  std::chrono::seconds                     rx_timeout{0};
  std::optional<std::chrono::milliseconds> rto_initial;
  std::optional<std::chrono::milliseconds> rto_min;
  std::optional<std::chrono::milliseconds> rto_max;
  std::optional<int32_t>                   init_max_attempts;
  std::optional<std::chrono::milliseconds> max_init_timeo;
  std::optional<std::chrono::milliseconds> hb_interval;
  std::optional<int32_t>                   assoc_max_rxt;
  std::optional<bool>                      nodelay;
};

/// SCTP socket instance.
class sctp_socket
{
public:
  static expected<sctp_socket> create(const sctp_socket_params& params);

  sctp_socket();
  sctp_socket(sctp_socket&& other) noexcept = default;
  sctp_socket& operator=(sctp_socket&& other) noexcept;

  bool close();

  [[nodiscard]] bool is_open() const { return sock_fd.is_open(); }
  const unique_fd&   fd() const { return sock_fd; }
  void               release()
  {
    int fd  = sock_fd.release();
    sock_fd = unique_fd(fd, false);
  }

  /// \brief Bind to one or many addresses using sctp_bindx(). If binding to one address, interface can be specified.
  [[nodiscard]] bool bindx(const std::vector<sockaddr_storage>& addrs, const std::string& bind_interface);
  /// \brief Connect to one or many addresses using sctp_connectx().
  /// \param addrs List of addresses to connect to.
  /// \param assoc_id Reference to receive the association ID returned by sctp_connectx.
  [[nodiscard]] bool connectx(const std::vector<sockaddr_storage>& addrs, sctp_assoc_t& assoc_id);
  /// \brief Start listening on socket.
  [[nodiscard]] bool listen();
  [[nodiscard]] bool set_non_blocking();

  /// \brief Return the local port the socket is bound to (0 if not yet bound, std::nullopt if socket closed).
  /// This can be used to check if socket is bound and to get the port number if dynamic port number was used.
  std::optional<uint16_t> get_bound_port() const;

  /// \brief Return the address family of the socket (AF_INET or AF_INET6).
  /// Returns std::nullopt if socket is closed, address family is unknown or getsockname fails.
  std::optional<int> get_address_family() const;

private:
  bool set_sockopts(const sctp_socket_params& params);

  std::string             if_name;
  bool                    non_blocking_mode = false;
  ocudulog::basic_logger& logger;

  unique_fd sock_fd;
};

} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::sctp_socket_params> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::sctp_socket_params& cfg, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(),
        "if_name={} ai_family={} ai_socktype={} reuse_addr={} non_blockin_mode={} rx_timeout={} "
        "rto_initial={} rto_min={} rto_max={} init_max_attempts={} max_init_timeo={} assoc_max_rtx={} no_delay={}",
        cfg.if_name,
        cfg.ai_family,
        cfg.ai_socktype,
        cfg.reuse_addr,
        cfg.non_blocking_mode,
        cfg.rx_timeout.count(),
        cfg.rto_initial,
        cfg.rto_min,
        cfg.rto_max,
        cfg.init_max_attempts,
        cfg.max_init_timeo,
        cfg.hb_interval,
        cfg.assoc_max_rxt,
        cfg.nodelay);
  }
};
} // namespace fmt
