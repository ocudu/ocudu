// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/lpp/lpp.h"

using namespace asn1;
using namespace asn1::lpp;

const char* ellipsoid_point_s::latitude_sign_opts::to_string() const
{
  static const char* names[] = {"north", "south"};
  return convert_enum_idx(names, 2, value, "latitude_sign_e");
}

// Ellipsoid-Point ::= SEQUENCE
OCUDUASN_CODE ellipsoid_point_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(latitude_sign.pack(bref));
  HANDLE_CODE(pack_integer(bref, degrees_latitude, (uint32_t)0u, (uint32_t)8388607u, false, false));
  HANDLE_CODE(pack_integer(bref, degrees_longitude, (int32_t)-8388608, (int32_t)8388607, false, false));

  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE ellipsoid_point_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(latitude_sign.unpack(bref));
  HANDLE_CODE(unpack_integer(degrees_latitude, bref, (uint32_t)0u, (uint32_t)8388607u, false, false));
  HANDLE_CODE(unpack_integer(degrees_longitude, bref, (int32_t)-8388608, (int32_t)8388607, false, false));

  return OCUDUASN_SUCCESS;
}
void ellipsoid_point_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("latitudeSign", latitude_sign.to_string());
  j.write_int("degreesLatitude", degrees_latitude);
  j.write_int("degreesLongitude", degrees_longitude);
  j.end_obj();
}
