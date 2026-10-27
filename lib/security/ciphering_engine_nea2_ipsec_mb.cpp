// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ciphering_engine_nea2_ipsec_mb.h"

#ifdef OCUDU_HAVE_IPSEC_MB

using namespace ocudu;
using namespace security;

ciphering_engine_nea2_ipsec_mb::ciphering_engine_nea2_ipsec_mb(sec_128_key        k_128_enc_,
                                                               uint8_t            bearer_id_,
                                                               security_direction direction_) :
  bearer_id(bearer_id_), direction(direction_), k_128_enc(k_128_enc_), logger(ocudulog::fetch_basic_logger("SEC"))
{
  mgr = alloc_mb_mgr(0);

  if (mgr == nullptr) {
    report_error("Failed to allocate IPsec-MB manager");
  }

  init_mb_mgr_auto(mgr, nullptr);
  IMB_AES_KEYEXP_128(mgr, k_128_enc.data(), expanded_enc_key.data(), expanded_dec_key.data());
}

ciphering_engine_nea2_ipsec_mb::~ciphering_engine_nea2_ipsec_mb()
{
  free_mb_mgr(mgr);
}

security_status ciphering_engine_nea2_ipsec_mb::apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count)
{
  byte_buffer_view msg{buf.begin() + offset, buf.end()};

  logger.debug("Applying ciphering. count={}", count);
  logger.debug("K_enc: {}", k_128_enc);
  logger.debug(msg.begin(), msg.end(), "Ciphering input:");

  // Linearize buffer.
  byte_buffer_segment_span_range segments  = msg.modifiable_segments();
  size_t                         cp_offset = 0;
  for (const auto& segment : segments) {
    ocudu_assert((segment.size()) <= LINEAR_BUFF_SZ - cp_offset,
                 "Overflowing linear buffer in NEA2 ciphering computation");
    std::memcpy(linear_buffer.data() + cp_offset, segment.data(), segment.size());
    cp_offset += segment.size();
  }

  // Prepare NONCE
  unsigned char nonce_cnt[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Construct nonce
  nonce_cnt[0] = (count >> 24) & 0xff;
  nonce_cnt[1] = (count >> 16) & 0xff;
  nonce_cnt[2] = (count >> 8) & 0xff;
  nonce_cnt[3] = count & 0xff;
  nonce_cnt[4] = ((bearer_id & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

  // Prepare job.
  IMB_JOB* job = IMB_GET_NEXT_JOB(mgr);

  job->cipher_direction = IMB_DIR_ENCRYPT;
  job->chain_order      = IMB_ORDER_CIPHER_HASH;
  job->cipher_mode      = IMB_CIPHER_CNTR;
  job->hash_alg         = IMB_AUTH_NULL;

  job->src                              = linear_buffer.data();
  job->dst                              = linear_buffer.data();
  job->msg_len_to_cipher_in_bytes       = msg.length();
  job->cipher_start_src_offset_in_bytes = 0;

  job->enc_keys         = expanded_enc_key.data();
  job->dec_keys         = expanded_enc_key.data();
  job->key_len_in_bytes = 16;

  job->iv              = nonce_cnt;
  job->iv_len_in_bytes = sizeof(nonce_cnt);

  // Submit the job.
  IMB_JOB* completed = IMB_SUBMIT_JOB(mgr);

  if (completed == nullptr) {
    completed = IMB_FLUSH_JOB(mgr);
  }

  if (completed == nullptr || completed->status != IMB_STATUS_COMPLETED) {
    return security_status::engine_failure;
  }
  // Copy encrypted data back into the segments
  cp_offset = 0;
  for (const auto& segment : segments) {
    ocudu_assert((segment.size()) <= LINEAR_BUFF_SZ - cp_offset,
                 "Overflowing linear buffer in NEA2 ciphering computation");
    std::memcpy(segment.data(), linear_buffer.data() + cp_offset, segment.size());
    cp_offset += segment.size();
  }

  // Copy back to byte buffer.
  logger.debug(msg.begin(), msg.end(), "Ciphering output:");

  return security_status::success;
}
#endif
