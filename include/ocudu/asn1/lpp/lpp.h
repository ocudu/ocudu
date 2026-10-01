// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                     3GPP TS 37.355 ASN1 LPP v18.1.0 (2024-05)
 *
 ******************************************************************************/

#pragma once
#include "ocudu/asn1/asn1_utils.h"

namespace asn1 {
namespace lpp {

// Ellipsoid-Point ::= SEQUENCE
// The IE Ellipsoid-Point is used to describe a geographic shape as defined in TS 23.032.
// Used in sib19_r17_s for ref_location_r17.
struct ellipsoid_point_s {
  struct latitude_sign_opts {
    enum options { north, south, nulltype } value;

    const char* to_string() const;
  };
  using latitude_sign_opts_e_ = enumerated<latitude_sign_opts>;

  latitude_sign_opts_e_ latitude_sign;
  uint32_t              degrees_latitude;  // INTEGER (0..8388607)
  int32_t               degrees_longitude; // INTEGER (-8388608..8388607)

  // Methods
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
};

} // namespace lpp
} // namespace asn1
