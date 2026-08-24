// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ciphering_engine_nea2_psa.h"

using namespace ocudu;
using namespace security;

#if MBEDTLS_VERSION_NUMBER >= 0x04000000
ciphering_engine_nea2_psa::ciphering_engine_nea2_psa(sec_128_key        k_128_enc_,
                                                     uint8_t            bearer_id_,
                                                     security_direction direction_) :
  bearer_id(bearer_id_), direction(direction_), k_128_enc(k_128_enc_), logger(ocudulog::fetch_basic_logger("SEC"))
{
  int ret = aes_setkey_enc(&ctx, k_128_enc.data(), 128);
  if (ret != 0) {
    ocudu_assertion_failure("Failure in aes_setkey_enc");
    return;
  }
}

security_status ciphering_engine_nea2_psa::apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count)
{
  // TODO.
  (void)bearer_id;
  (void)direction;

  return security_status::success;
}
#endif
