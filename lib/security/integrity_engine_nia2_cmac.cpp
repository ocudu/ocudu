// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "integrity_engine_nia2_cmac.h"
#include "mbedtls/cmac.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace security;

#ifdef MBEDTLS_CMAC_C

integrity_engine_nia2_cmac::integrity_engine_nia2_cmac(sec_128_key        k_128_int_,
                                                       uint8_t            bearer_id_,
                                                       security_direction direction_) :
  k_128_int(k_128_int_), bearer_id(bearer_id_), direction(direction_), logger(ocudulog::fetch_basic_logger("SEC"))
{
  cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
  if (cipher_info == nullptr) {
    ocudu_assertion_failure("Failure in mbedtls_cipher_info_from_type");
    return;
  }
  mbedtls_cipher_init(&ctx);

  int ret;
  ret = mbedtls_cipher_setup(&ctx, cipher_info);
  if (ret != 0) {
    ocudu_assertion_failure("Failure in mbedtls_cipher_setup");
    return;
  }

  ret = mbedtls_cipher_cmac_starts(&ctx, k_128_int.data(), 128);
  if (ret != 0) {
    ocudu_assertion_failure("Failure in mbedtls_cipher_cmac_starts");
    return;
  }
}

integrity_engine_nia2_cmac::~integrity_engine_nia2_cmac()
{
  mbedtls_cipher_free(&ctx);
}

security_status integrity_engine_nia2_cmac::compute_mac(sec_mac& mac, const byte_buffer_view v, uint32_t count)
{
  // reset state machine
  int ret;
  ret = mbedtls_cipher_cmac_reset(&ctx);
  if (ret != 0) {
    return security_status::integrity_failure;
  }

  // process preamble
  std::array<uint8_t, 8> preamble = {};
  preamble[0]                     = (count >> 24) & 0xff;
  preamble[1]                     = (count >> 16) & 0xff;
  preamble[2]                     = (count >> 8) & 0xff;
  preamble[3]                     = count & 0xff;
  preamble[4]                     = (bearer_id << 3) | (to_number(direction) << 2);
  ret                             = mbedtls_cipher_cmac_update(&ctx, preamble.data(), preamble.size());
  if (ret != 0) {
    return security_status::integrity_failure;
  }

  // process PDU segments
  const_byte_buffer_segment_span_range segments = v.segments();
  for (const auto& segment : segments) {
    ret = mbedtls_cipher_cmac_update(&ctx, segment.data(), segment.size());
    if (ret != 0) {
      return security_status::integrity_failure;
    }
  }

  // complete CMAC computation
  std::array<uint8_t, 16> tmp_mac;
  ret = mbedtls_cipher_cmac_finish(&ctx, tmp_mac.data());
  if (ret != 0) {
    return security_status::integrity_failure;
  }

  // copy first 4 bytes
  std::copy(tmp_mac.begin(), tmp_mac.begin() + 4, mac.begin());

  return security_status::success;
}

security_status integrity_engine_nia2_cmac::protect_integrity(byte_buffer& buf, uint32_t count)
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
  logger.debug("MAC: {}", mac);
  logger.debug(buf.begin(), buf.end(), "Message output:");

  return security_status::success;
}

security_status integrity_engine_nia2_cmac::verify_integrity(byte_buffer& buf, uint32_t count)
{
  if (buf.length() <= sec_mac_len) {
    return security_status::integrity_failure;
  }

  byte_buffer_view v{buf, 0, buf.length() - sec_mac_len};
  byte_buffer_view m{buf, buf.length() - sec_mac_len, sec_mac_len};

  // compute MAC
  security::sec_mac mac    = {};
  security_status   status = compute_mac(mac, v, count);

  if (status != security_status::success) {
    return status;
  }

  // verify MAC
  if (!std::equal(mac.begin(), mac.end(), m.begin(), m.end())) {
    security::sec_mac mac_rx;
    std::copy(m.begin(), m.end(), mac_rx.begin());
    logger.warning("Integrity check failed. count={}", count);
    logger.warning("K_int: {}", k_128_int);
    logger.warning("MAC received: {}", mac_rx);
    logger.warning("MAC expected: {}", mac);
    logger.warning(v.begin(), v.end(), "Message input:");
    return security_status::integrity_failure;
  }
  logger.debug("Integrity check passed. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC: {}", mac);
  logger.debug(v.begin(), v.end(), "Message input:");

  // trim MAC from PDU
  buf.trim_tail(sec_mac_len);

  return security_status::success;
}

#endif // MBEDTLS_CMAC_C
