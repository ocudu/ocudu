// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/

#include "security.h"
#include "ocudu/asn1/asn1_utils.h"
#include <cstdint>

namespace ocudu::security {

inline void asn1_to_key(security::sec_key& key_out, const asn1::fixed_bitstring<256, false, true>& key_in)
{
  for (uint32_t i = 0; i < key_in.nof_octets(); ++i) {
    key_out[i] = key_in.data()[key_in.nof_octets() - 1 - i];
  }
}

inline asn1::fixed_bitstring<256, false, true> key_to_asn1(const security::sec_key& key_in)
{
  asn1::fixed_bitstring<256, false, true> asn1_key;
  for (uint32_t i = 0; i < asn1_key.nof_octets(); ++i) {
    asn1_key.data()[asn1_key.nof_octets() - 1 - i] = key_in[i];
  }
  return asn1_key;
}

inline void asn1_to_supported_algorithms(security::supported_algorithms&              supported_algos_out,
                                         const asn1::fixed_bitstring<16, true, true>& supported_algos_in)
{
  uint16_t tmp = *reinterpret_cast<const uint16_t*>(supported_algos_in.data());
  for (uint16_t i = 0; i < 3; ++i) {
    supported_algos_out[i] = ((tmp >> (15 - i)) & 0x01U) == 1; // interesting bits are in the second byte
  }
}

inline asn1::fixed_bitstring<16, true, true>
supported_algorithms_to_asn1(const security::supported_algorithms& supported_algos)
{
  asn1::fixed_bitstring<16, true, true> asn1_supported_algos;
  uint16_t                              tmp = 0;
  for (uint16_t i = 0; i < 3; ++i) {
    tmp |= (supported_algos[i] ? 1 : 0) << (15 - i);
  }
  *reinterpret_cast<uint16_t*>(asn1_supported_algos.data()) = tmp;
  return asn1_supported_algos;
}

} // namespace ocudu::security
