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
  constexpr uint32_t lat_max = 8388607u;
  constexpr int32_t  lon_min = -8388608;
  constexpr int32_t  lon_max = 8388607;

  asn1::lpp::ellipsoid_point_s ep;
  ep.latitude_sign = (loc.latitude < 0.0) ? asn1::lpp::ellipsoid_point_s::latitude_sign_opts::south
                                          : asn1::lpp::ellipsoid_point_s::latitude_sign_opts::north;
  ep.degrees_latitude =
      std::clamp(static_cast<uint32_t>(std::floor(std::abs(loc.latitude) * lat_max / 90.0)), 0u, lat_max);
  ep.degrees_longitude =
      std::clamp(static_cast<int32_t>(std::floor(loc.longitude * 16777215.0 / 360.0)), lon_min, lon_max);

  byte_buffer         buf;
  asn1::bit_ref       bref{buf};
  asn1::OCUDUASN_CODE ret = ep.pack(bref);
  ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack LPP Ellipsoid-Point reference location");
  return buf;
}
