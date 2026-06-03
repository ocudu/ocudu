// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gtpu/gtpu_teid.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/io/transport_layer_address.h"
#include <algorithm>
#include <array>

namespace ocudu {

/// \brief Creates a transport_layer_address from an ASN.1 TransportLayerAddress bitstring field.
/// The ASN.1 bitstring stores bytes in reverse of network byte order (octets_[0] holds the lowest-index bits), so the
/// bytes are reversed into the network-order layout expected by transport_layer_address.
template <typename Asn1Bitstring>
transport_layer_address tla_from_asn1_bitstring(const Asn1Bitstring& bs)
{
  std::array<uint8_t, 20> ip_bytes{};
  report_fatal_error_if_not(bs.nof_octets() <= ip_bytes.size(),
                            "Bitstring octet count {} exceeds maximum supported size {}",
                            bs.nof_octets(),
                            ip_bytes.size());
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
  std::reverse_copy(bs.data(), bs.data() + bs.nof_octets(), ip_bytes.begin());
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
  return transport_layer_address::create_from_bytes({ip_bytes.data(), bs.nof_octets()});
}

/// \brief Stores a transport_layer_address into an ASN.1 TransportLayerAddress bitstring field.
/// The ASN.1 bitstring stores bytes in reverse of network byte order (octets_[0] holds the lowest-index bits), so the
/// bytes are reversed from the network-order layout used by transport_layer_address.
template <typename Asn1Bitstring>
void tla_to_asn1_bitstring(Asn1Bitstring& bs, const transport_layer_address& tla)
{
  auto ip_bytes = tla.to_bytes();
  bs.resize(ip_bytes.size() * 8);
  std::reverse_copy(ip_bytes.begin(), ip_bytes.end(), bs.data());
}

/// \brief Identifier for F1-U transport layer associated to a DRB.
struct up_transport_layer_info {
  transport_layer_address tp_address = transport_layer_address::create_from_string("0.0.0.0");
  gtpu_teid_t             gtp_teid;

  up_transport_layer_info() = default;

  up_transport_layer_info(const transport_layer_address& tp_address_, gtpu_teid_t gtp_teid_) :
    tp_address(tp_address_), gtp_teid(gtp_teid_)
  {
  }

  bool operator==(const up_transport_layer_info& other) const
  {
    return tp_address == other.tp_address and gtp_teid == other.gtp_teid;
  }

  bool operator!=(const up_transport_layer_info& other) const { return not(*this == other); }

  bool operator<(const up_transport_layer_info& other) const
  {
    return gtp_teid < other.gtp_teid or (gtp_teid == other.gtp_teid and tp_address < other.tp_address);
  }
};

/// \brief Converts type \c up_transport_layer_info to an ASN.1 type.
/// \param asn1obj ASN.1 object where the result of the conversion is stored.
/// \param up_tp_layer_info UP Transport Layer Info object.
template <typename Asn1Type>
void up_transport_layer_info_to_asn1(Asn1Type& asn1obj, const up_transport_layer_info& up_tp_layer_info)
{
  asn1obj.set_gtp_tunnel();
  asn1obj.gtp_tunnel().gtp_teid.from_number(up_tp_layer_info.gtp_teid.value());
  tla_to_asn1_bitstring(asn1obj.gtp_tunnel().transport_layer_address, up_tp_layer_info.tp_address);
}

/// \brief Converts ASN.1 type into \c up_transport_layer_info.
/// \param asn1obj ASN.1 object which is going to be converted.
/// \return UP Transport Layer Info object where the result of the conversion is stored.
template <typename Asn1Type>
up_transport_layer_info asn1_to_up_transport_layer_info(const Asn1Type& asn1obj)
{
  return {tla_from_asn1_bitstring(asn1obj.gtp_tunnel().transport_layer_address),
          int_to_gtpu_teid(asn1obj.gtp_tunnel().gtp_teid.to_number())};
}

} // namespace ocudu

namespace std {

template <>
struct hash<ocudu::up_transport_layer_info> {
  size_t operator()(const ocudu::up_transport_layer_info& s) const noexcept
  {
    return (hash<ocudu::transport_layer_address>{}(s.tp_address) ^
            (hash<ocudu::gtpu_teid_t::value_type>{}(s.gtp_teid.value()) << 1U) >> 1U);
  }
};

} // namespace std

namespace fmt {

template <>
struct formatter<ocudu::up_transport_layer_info> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const ocudu::up_transport_layer_info& s, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{{Addr={} TEID={}}}", s.tp_address, s.gtp_teid);
  }
};

} // namespace fmt
