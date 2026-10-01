// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/io/transport_layer_address.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

transport_layer_address::transport_layer_address(const struct sockaddr& addr_, socklen_t socklen) : addrlen(socklen)
{
  std::memcpy((struct sockaddr*)&addr_storage, &addr_, addrlen);
}

transport_layer_address transport_layer_address::create_from_sockaddr(const struct sockaddr& sockaddr,
                                                                      socklen_t              addr_len)
{
  return transport_layer_address(sockaddr, addr_len);
}

transport_layer_address transport_layer_address::create_from_sockaddr(native_type addr)
{
  return transport_layer_address(*addr.addr, addr.addrlen);
}

transport_layer_address transport_layer_address::create_from_string(const std::string& ip_str)
{
  ::addrinfo* results;

  int err = ::getaddrinfo(ip_str.c_str(), nullptr, nullptr, &results);
  ocudu_assert(err == 0, "Getaddrinfo error: {} - {}", ip_str, ::gai_strerror(err));

  transport_layer_address res(*results->ai_addr, results->ai_addrlen);

  ::freeaddrinfo(results);

  return res;
}

transport_layer_address transport_layer_address::create_from_bytes(span<const uint8_t> ip_bytes)
{
  // See TS 38.414: 4 bytes (32 bits) for IPv4 (RFC 791), 16 bytes (128 bits) for IPv6 (RFC 8200), 20 bytes (160 bits)
  // for combined IPv4+IPv6 where the IPv4 address is contained in the first 4 bytes.
  // TODO: Support 20-byte transport layer addresses.
  ocudu_assert(ip_bytes.size() == 4 || ip_bytes.size() == 16,
               "Unsupported TransportLayerAddress byte length: {} (expected 4 or 16)",
               ip_bytes.size());

  if (ip_bytes.size() == 4) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr, ip_bytes.data(), 4);
    return create_from_sockaddr(reinterpret_cast<const sockaddr&>(addr), sizeof(addr));
  }

  sockaddr_in6 addr{};
  addr.sin6_family = AF_INET6;
  std::memcpy(&addr.sin6_addr, ip_bytes.data(), 16);
  return create_from_sockaddr(reinterpret_cast<const sockaddr&>(addr), sizeof(addr));
}

span<const uint8_t> transport_layer_address::to_bytes() const
{
  const auto* saddr = reinterpret_cast<const sockaddr*>(&addr_storage);

  if (saddr->sa_family == AF_INET) {
    const auto* addr = reinterpret_cast<const sockaddr_in*>(saddr);
    return {reinterpret_cast<const uint8_t*>(&addr->sin_addr), 4};
  }

  if (saddr->sa_family == AF_INET6) {
    const auto* addr = reinterpret_cast<const sockaddr_in6*>(saddr);
    return {reinterpret_cast<const uint8_t*>(&addr->sin6_addr), 16};
  }

  return {};
}

bool transport_layer_address::operator==(const transport_layer_address& other) const
{
  if (empty() && other.empty()) {
    return true;
  }
  return sockaddr_storage_equal(addr_storage, other.addr_storage);
}

bool transport_layer_address::operator<(const transport_layer_address& other) const
{
  return sockaddr_storage_less{}(addr_storage, other.addr_storage);
}
