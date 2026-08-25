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
  int ret = crypto_init();
  if (ret != 0) {
    report_error("Failure in initializing crypto PSA");
    return;
  }
  ret = aes_setkey_enc(&ctx, k_128_enc.data(), 128);
  if (ret != 0) {
    report_error("Failure in setting AES security key");
    return;
  }
}

security_status ciphering_engine_nea2_psa::apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count)
{
  byte_buffer_view msg{buf.begin() + offset, buf.end()};

  logger.debug("Applying ciphering. count={}", count);
  logger.debug("K_enc: {}", k_128_enc);
  logger.debug(msg.begin(), msg.end(), "Ciphering input:");

  unsigned char nonce_cnt[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Construct nonce
  nonce_cnt[0] = (count >> 24) & 0xff;
  nonce_cnt[1] = (count >> 16) & 0xff;
  nonce_cnt[2] = (count >> 8) & 0xff;
  nonce_cnt[3] = count & 0xff;
  nonce_cnt[4] = ((bearer_id & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

  // Encryption
  psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
  psa_status_t           status    = psa_cipher_encrypt_setup(&operation, ctx.ctr_key_id, PSA_ALG_CTR);
  if (status != PSA_SUCCESS) {
    return security_status::ciphering_failure;
  }

  status = psa_cipher_set_iv(&operation, nonce_cnt, sizeof(nonce_cnt));
  if (status != PSA_SUCCESS) {
    psa_cipher_abort(&operation);
    return security_status::ciphering_failure;
  }

  byte_buffer_segment_span_range segments = msg.modifiable_segments();
  for (const auto& segment : segments) {
    size_t output_len = 0;
    status = psa_cipher_update(&operation, segment.data(), segment.size(), segment.data(), segment.size(), &output_len);
    if (status != PSA_SUCCESS || output_len != segment.size()) {
      psa_cipher_abort(&operation);
      return security_status::ciphering_failure;
    }
  }

  unsigned char output[16];
  size_t        output_len = 0;
  status                   = psa_cipher_finish(&operation, output, sizeof(output), &output_len);
  if (status != PSA_SUCCESS || output_len != 0) {
    return security_status::ciphering_failure;
  }

  logger.debug(msg.begin(), msg.end(), "Ciphering output:");

  return security_status::success;
}
#endif
