// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "integrity_engine_nia2_psa.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace security;

#if MBEDTLS_VERSION_NUMBER >= 0x04000000

integrity_engine_nia2_psa::integrity_engine_nia2_psa(sec_128_key        k_128_int_,
                                                     uint8_t            bearer_id_,
                                                     security_direction direction_,
                                                     bool               allow_unprotected_) :
  k_128_int(k_128_int_),
  bearer_id(bearer_id_),
  direction(direction_),
  logger(ocudulog::fetch_basic_logger("SEC")),
  allow_unprotected(allow_unprotected_)
{
  int ret = crypto_init();
  if (ret != 0) {
    report_error("Failure in initializing crypto PSA");
    return;
  }
  ret = aes_setkey_enc(&ctx, k_128_int.data(), 128);
  if (ret != 0) {
    report_error("Failure in setting AES key");
    return;
  }
}

integrity_engine_nia2_psa::~integrity_engine_nia2_psa() {}

security_status integrity_engine_nia2_psa::compute_mac(sec_mac& mac, const byte_buffer_view v, uint32_t count)
{
  psa_mac_operation_t op     = PSA_MAC_OPERATION_INIT;
  psa_status_t        status = psa_mac_sign_setup(&op, ctx.cmac_key_id, PSA_ALG_CMAC);
  if (status != PSA_SUCCESS) {
    return security_status::engine_failure;
  }

  // process preamble
  std::array<uint8_t, 8> preamble = {};
  preamble[0]                     = (count >> 24) & 0xff;
  preamble[1]                     = (count >> 16) & 0xff;
  preamble[2]                     = (count >> 8) & 0xff;
  preamble[3]                     = count & 0xff;
  preamble[4]                     = (bearer_id << 3) | (to_number(direction) << 2);
  status                          = psa_mac_update(&op, preamble.data(), preamble.size());

  if (status != PSA_SUCCESS) {
    psa_mac_abort(&op);
    return security_status::integrity_failure;
  }

  // process PDU segments
  const_byte_buffer_segment_span_range segments = v.segments();
  for (const auto& segment : segments) {
    status = psa_mac_update(&op, segment.data(), segment.size());
    if (status != PSA_SUCCESS) {
      psa_mac_abort(&op);
      return security_status::integrity_failure;
    }
  }

  // complete CMAC computation
  std::array<uint8_t, 16> tmp_mac;
  size_t                  mac_len = 0;
  status                          = psa_mac_sign_finish(&op, tmp_mac.data(), tmp_mac.size(), &mac_len);

  if (status != PSA_SUCCESS || mac_len != tmp_mac.size()) {
    return security_status::integrity_failure;
  }

  // Copy first 4 bytes.
  std::copy(tmp_mac.begin(), tmp_mac.begin() + 4, mac.begin());
  return security_status::success;
}

security_status integrity_engine_nia2_psa::protect_integrity(byte_buffer& buf, uint32_t count)
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

security_status integrity_engine_nia2_psa::verify_integrity(byte_buffer& buf, uint32_t count)
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
