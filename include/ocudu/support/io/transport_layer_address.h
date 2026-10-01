// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/support/io/sockets.h"
#include "fmt/format.h"
#include <string>

namespace ocudu {

/// \brief Representation of an Transport Layer Address.
class transport_layer_address
{
  using storage_type = ::sockaddr_storage;

public:
  /// Underlying native type used to store a transport layer address.
  struct native_type {
    struct sockaddr* addr;
    socklen_t        addrlen; // TO-DO: we should not store that, derive it from ss_family instead?
  };

  transport_layer_address() = default;

  /// Creates a transport_layer_address from a numeric IPv4 or IPv6 address or hostname string.
  /// Supports standard IPv6 notation, including compressed forms (e.g. "2001:db8::1").
  static transport_layer_address create_from_string(const std::string& ip_str);

  /// Creates a transport_layer_address from raw IPv4 or IPv6 bytes in network byte order (4 bytes for IPv4, 16 for
  /// IPv6, as per 3GPP TS 38.414). 160-bit combined IPv4+IPv6 is not yet supported.
  static transport_layer_address create_from_bytes(span<const uint8_t> ip_bytes);

  static transport_layer_address create_from_sockaddr(const struct sockaddr& sockaddr, socklen_t addr_len);
  static transport_layer_address create_from_sockaddr(native_type addr);

  bool empty() const { return addrlen == 0; }

  /// Converts the transport_layer_address to an IPv4 or IPv6 string.
  std::string to_string() const { return fmt::format("{}", *this); }

  /// Returns a view of the IPv4 or IPv6 bytes in network byte order (4 bytes for IPv4, 16 for IPv6). Returns an empty
  /// span for uninitialized addresses or unsupported address families.
  span<const uint8_t> to_bytes() const;

  /// Extracts the POSIX representation of the transport layer address.
  native_type native() const { return {(struct sockaddr*)&addr_storage, addrlen}; }

  /// Sets the port number of the transport layer address.
  bool set_port(uint16_t port)
  {
    if (((struct sockaddr*)&addr_storage)->sa_family == AF_INET) {
      ((struct sockaddr_in*)&addr_storage)->sin_port = htons(port);
    } else if (((struct sockaddr*)&addr_storage)->sa_family == AF_INET6) {
      ((struct sockaddr_in6*)&addr_storage)->sin6_port = htons(port);
    } else {
      return false;
    }
    return true;
  }

  /// Compares two transport_layer_addresses.
  bool operator==(const transport_layer_address& other) const;
  bool operator!=(const transport_layer_address& other) const { return not(*this == other); }
  bool operator==(const std::string& ip_str) const { return *this == create_from_string(ip_str); }
  bool operator!=(const std::string& ip_str) const { return not(*this == ip_str); }

  bool operator<(const transport_layer_address& other) const;
  bool operator<=(const transport_layer_address& other) const { return *this < other or *this == other; }
  bool operator>=(const transport_layer_address& other) const { return not(*this < other); }
  bool operator>(const transport_layer_address& other) const { return not(*this <= other); }

private:
  explicit transport_layer_address(const struct sockaddr& addr_, socklen_t socklen);

  storage_type addr_storage = {};
  socklen_t    addrlen      = 0;
};

} // namespace ocudu

namespace std {
template <>
struct hash<ocudu::transport_layer_address> {
  size_t operator()(const ocudu::transport_layer_address& s) const noexcept { return hash<string>{}(s.to_string()); }
};
} // namespace std

namespace fmt {

template <>
struct formatter<ocudu::transport_layer_address> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const ocudu::transport_layer_address& s, FormatContext& ctx) const
  {
    std::array<char, NI_MAXHOST> ip_addr;
    int                          port;
    auto                         native_repr = s.native();
    if (not ocudu::getnameinfo(*native_repr.addr, native_repr.addrlen, ip_addr, port)) {
      return format_to(ctx.out(), "invalid");
    }
    if (port != 0) {
      return format_to(ctx.out(), "{}:{}", ip_addr.data(), port);
    }
    return format_to(ctx.out(), "{}", ip_addr.data());
  }
};

} // namespace fmt
