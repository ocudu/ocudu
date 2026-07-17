// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
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
