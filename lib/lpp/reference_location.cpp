// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/lpp/reference_location.h"
#include "ocudu/asn1/lpp/lpp.h"
#include "ocudu/support/error_handling.h"
#include <algorithm>
#include <cmath>

using namespace ocudu;

byte_buffer lpp::pack_reference_location(const reference_location& loc)
{
  // TS 23.032 sec. 6.1 scale factors: N = floor(2^23/90 * |lat|) and floor(2^24/360 * lon).
  constexpr double   lat_scale = 8388608.0;  // 2^23
  constexpr double   lon_scale = 16777216.0; // 2^24
  constexpr uint32_t lat_max   = 8388607u;   // 2^23 - 1 (degreesLatitude is 23-bit unsigned)
  constexpr int32_t  lon_min   = -8388608;   // -2^23
  constexpr int32_t  lon_max   = 8388607;    // 2^23 - 1 (degreesLongitude is 24-bit two's complement)

  asn1::lpp::ellipsoid_point_s ep;
  ep.latitude_sign = (loc.latitude < 0.0) ? asn1::lpp::ellipsoid_point_s::latitude_sign_opts::south
                                          : asn1::lpp::ellipsoid_point_s::latitude_sign_opts::north;
  // N=2^23 (|lat|=90) is folded into the max code 2^23-1 per the spec's extended-range note for N=2^23-1.
  ep.degrees_latitude =
      std::clamp(static_cast<uint32_t>(std::floor(std::abs(loc.latitude) * lat_scale / 90.0)), 0u, lat_max);
  ep.degrees_longitude =
      std::clamp(static_cast<int32_t>(std::floor(loc.longitude * lon_scale / 360.0)), lon_min, lon_max);

  byte_buffer         buf;
  asn1::bit_ref       bref{buf};
  asn1::OCUDUASN_CODE ret = ep.pack(bref);
  ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack LPP Ellipsoid-Point reference location");
  return buf;
}

std::optional<reference_location> lpp::unpack_reference_location(const byte_buffer& packed)
{
  // 1 sign bit plus 23 and 24 coordinate bits. A longer buffer is not an Ellipsoid-Point, and decoding its first 48
  // bits would yield a plausible but wrong position.
  constexpr size_t ellipsoid_point_size = 6;
  if (packed.length() != ellipsoid_point_size) {
    return std::nullopt;
  }

  asn1::cbit_ref               bref{packed};
  asn1::lpp::ellipsoid_point_s ep;
  if (ep.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    return std::nullopt;
  }

  // TS 23.032 sec. 6.1: N <= 2^23 * |lat| / 90 < N+1 and N <= 2^24 * lon / 360 < N+1.
  constexpr double lat_scale = 8388608.0;  // 2^23
  constexpr double lon_scale = 16777216.0; // 2^24

  reference_location loc;
  loc.latitude = static_cast<double>(ep.degrees_latitude) * 90.0 / lat_scale;
  if (ep.latitude_sign == asn1::lpp::ellipsoid_point_s::latitude_sign_opts::south) {
    loc.latitude = -loc.latitude;
  }
  loc.longitude = static_cast<double>(ep.degrees_longitude) * 360.0 / lon_scale;
  return loc;
}
