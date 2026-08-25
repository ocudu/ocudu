// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#define OCUDU_MBEDTLS_PSA (MBEDTLS_VERSION_NUMBER >= 0x04000000)

#include "ocudu/support/error_handling.h"
#include <mbedtls/version.h>
#if OCUDU_MBEDTLS_PSA
#include <psa/crypto.h>
#else
#include <mbedtls/aes.h>
#include <mbedtls/cmac.h>
#include <mbedtls/md.h>
#endif

namespace ocudu::security {

constexpr int aes_encrypt = 1;
constexpr int aes_decrypt = 0;

#if OCUDU_MBEDTLS_PSA
struct aes_context {
  psa_key_id_t cmac_key_id = 0;
  psa_key_id_t ecb_key_id  = 0;
  psa_key_id_t ctr_key_id  = 0;

  aes_context()                              = default;
  aes_context(const aes_context&)            = delete;
  aes_context& operator=(const aes_context&) = delete;

  ~aes_context()
  {
    if (cmac_key_id != 0) {
      psa_destroy_key(cmac_key_id);
    }
    if (ecb_key_id != 0) {
      psa_destroy_key(ecb_key_id);
    }
    if (ctr_key_id != 0) {
      psa_destroy_key(ctr_key_id);
    }
  }
};
#else
using aes_context = mbedtls_aes_context;
#endif

inline int crypto_init()
{
#if OCUDU_MBEDTLS_PSA
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    return -1;
  }
#endif
  return 0;
}

inline int aes_setkey_enc(aes_context* ctx, const unsigned char* key, unsigned keysize)
{
#if OCUDU_MBEDTLS_PSA
  /// Set ECB PSA key.
  {
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, keysize);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);

    psa_status_t status = psa_import_key(&attributes, key, keysize / 8, &ctx->ecb_key_id);

    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
      return -1;
    }
  }
  /// Set CMAC PSA key.
  {
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, keysize);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);

    psa_status_t status = psa_import_key(&attributes, key, keysize / 8, &ctx->cmac_key_id);

    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
      return -1;
    }
  }
  /// Set CTR PSA key.
  {
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, keysize);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);

    psa_status_t status = psa_import_key(&attributes, key, keysize / 8, &ctx->ctr_key_id);

    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
      return -1;
    }
  }
  return 0;
#else
  return mbedtls_aes_setkey_enc(ctx, key, keysize);
#endif
}

inline int aes_crypt_ecb(aes_context* ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
#if OCUDU_MBEDTLS_PSA
  size_t       output_len = 0;
  psa_status_t status;

  if (mode == aes_encrypt) {
    status = psa_cipher_encrypt(ctx->ecb_key_id, PSA_ALG_ECB_NO_PADDING, input, 16, output, 16, &output_len);
  } else {
    status = psa_cipher_decrypt(ctx->ecb_key_id, PSA_ALG_ECB_NO_PADDING, input, 16, output, 16, &output_len);
  }

  return (status == PSA_SUCCESS && output_len == 16) ? 0 : -1;
#else
  return mbedtls_aes_crypt_ecb(ctx, mode, input, output);
#endif
}

inline void sha256(const unsigned char* key,
                   size_t               keylen,
                   const unsigned char* input,
                   size_t               ilen,
                   unsigned char        output[32],
                   int                  is224)
{
#if OCUDU_MBEDTLS_PSA
  int ret = crypto_init();
  if (ret != 0) {
    report_error("Failure in initializing crypto PSA");
    return;
  }

  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_key_id_t         key_id     = 0;

  psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
  psa_set_key_bits(&attributes, keylen * 8);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
  psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));

  psa_status_t status = psa_import_key(&attributes, key, keylen, &key_id);

  psa_reset_key_attributes(&attributes);

  if (status != PSA_SUCCESS) {
    return;
  }

  size_t output_len = 0;

  status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), input, ilen, output, 32, &output_len);

  psa_destroy_key(key_id);
#else
  mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key, keylen, input, ilen, output);
#endif
}

} // namespace ocudu::security
