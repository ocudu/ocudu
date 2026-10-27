// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "integrity_engine_nia2_ipsec_mb.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace security;

#if OCUDU_HAVE_IPSEC_MB

integrity_engine_nia2_ipsec_mb::integrity_engine_nia2_ipsec_mb(sec_128_key        k_128_int_,
                                                               uint8_t            bearer_id_,
                                                               security_direction direction_,
                                                               bool               allow_unprotected_) :
  k_128_int(k_128_int_),
  bearer_id(bearer_id_),
  direction(direction_),
  logger(ocudulog::fetch_basic_logger("SEC")),
  allow_unprotected(allow_unprotected_)
{
  mgr = alloc_mb_mgr(0);

  if (mgr == nullptr) {
    report_error("Failed to allocate IPsec-MB manager");
  }

  init_mb_mgr_auto(mgr, nullptr);
  IMB_AES_KEYEXP_128(mgr, k_128_int.data(), expanded_enc_key.data(), expanded_dec_key.data());
  IMB_AES_CMAC_SUBKEY_GEN_128(mgr, expanded_enc_key.data(), subkey1.data(), subkey2.data());
}

integrity_engine_nia2_ipsec_mb::~integrity_engine_nia2_ipsec_mb()
{
  free_mb_mgr(mgr);
}

security_status integrity_engine_nia2_ipsec_mb::compute_mac(sec_mac& mac, const byte_buffer_view v, uint32_t count)
{
  IMB_JOB* job = IMB_GET_NEXT_JOB(mgr);

  job->cipher_mode      = IMB_CIPHER_NULL;
  job->cipher_direction = IMB_DIR_ENCRYPT;
  job->hash_alg         = IMB_AUTH_AES_CMAC;
  job->chain_order      = IMB_ORDER_HASH_CIPHER;
  job->key_len_in_bytes = 16;

  job->u.CMAC._key_expanded = expanded_enc_key.data();
  job->u.CMAC._skey1        = subkey1.data();
  job->u.CMAC._skey2        = subkey2.data();

  linear_buffer[0] = (count >> 24) & 0xff;
  linear_buffer[1] = (count >> 16) & 0xff;
  linear_buffer[2] = (count >> 8) & 0xff;
  linear_buffer[3] = count & 0xff;
  linear_buffer[4] = (bearer_id << 3) | (to_number(direction) << 2);
  linear_buffer[5] = 0;
  linear_buffer[6] = 0;
  linear_buffer[7] = 0;

  size_t offset = PREAMBLE_SZ;
  for (const auto& segment : v.segments()) {
    ocudu_assert((segment.size()) <= LINEAR_BUFF_SZ - offset, "Overflowing linear buffer in NIA2 MAC-I computation");
    std::memcpy(linear_buffer.data() + offset, segment.data(), segment.size());
    offset += segment.size();
  }

  job->src                            = linear_buffer.data();
  job->hash_start_src_offset_in_bytes = 0;
  job->msg_len_to_hash_in_bytes       = offset;

  std::array<uint8_t, sec_mac_len> tmp_mac;
  job->auth_tag_output              = tmp_mac.data();
  job->auth_tag_output_len_in_bytes = tmp_mac.size();

  IMB_JOB* completed = IMB_SUBMIT_JOB(mgr);

  if (completed == nullptr) {
    completed = IMB_FLUSH_JOB(mgr);
  }

  if (completed == nullptr || completed->status != IMB_STATUS_COMPLETED) {
    return security_status::engine_failure;
  }
  std::copy_n(tmp_mac.begin(), mac.size(), mac.begin());
  return security_status::success;
}

security_status integrity_engine_nia2_ipsec_mb::protect_integrity(byte_buffer& buf, uint32_t count)
{
  byte_buffer_view v{buf.begin(), buf.end()};

  security::sec_mac mac    = {};
  security_status   status = compute_mac(mac, v, count);

  logger.debug("Applying integrity protection. count={}", count);
  logger.debug(v.begin(), v.end(), "Message input:");

  if (status != security_status::success) {
    return status;
  }

  if (not buf.append(mac)) {
    return security_status::buffer_failure;
  }

  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC-I: {}", mac);
  logger.debug(buf.begin(), buf.end(), "Message output:");

  return security_status::success;
}

security_status integrity_engine_nia2_ipsec_mb::verify_integrity(byte_buffer& buf, uint32_t count)
{
  if (buf.length() <= sec_mac_len) {
    return security_status::integrity_failure;
  }

  byte_buffer_view v{buf, 0, buf.length() - sec_mac_len};
  byte_buffer_view m{buf, buf.length() - sec_mac_len, sec_mac_len};

  // Compute MAC-I.
  security::sec_mac mac    = {};
  security_status   status = compute_mac(mac, v, count);

  if (status != security_status::success) {
    return status;
  }

  // Verify MAC-I.
  if (!std::equal(mac.begin(), mac.end(), m.begin(), m.end())) {
    if (allow_unprotected) {
      // Unprotected PDUs are expected to fail the integrity check but must have zero MAC-I.
      static constexpr security::sec_mac zero_mac = {};
      if (std::equal(zero_mac.begin(), zero_mac.end(), m.begin(), m.end())) {
        // Integrity passed (as unprotected).
        logger.debug("Integrity check passed as unprotected with zero MAC-I. count={}", count);
        logger.debug("K_int: {}", k_128_int);
        logger.debug(v.begin(), v.end(), "Message input:");

        // Trim MAC-I from PDU.
        buf.trim_tail(sec_mac_len);
        return security_status::success_unprotected;
      }
    }
    // Integrity failure.
    security::sec_mac mac_rx;
    std::copy(m.begin(), m.end(), mac_rx.begin());
    logger.warning("Integrity check failed. count={}", count);
    logger.warning("K_int: {}", k_128_int);
    logger.warning("MAC-I received: {}", mac_rx);
    logger.warning("MAC-I expected: {}", mac);
    logger.warning(v.begin(), v.end(), "Message input:");
    return security_status::integrity_failure;
  }
  // Integrity passed (as protected).
  logger.debug("Integrity check passed. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC-I: {}", mac);
  logger.debug(v.begin(), v.end(), "Message input:");

  // Trim MAC-I from PDU.
  buf.trim_tail(sec_mac_len);
  return security_status::success;
}

#endif
