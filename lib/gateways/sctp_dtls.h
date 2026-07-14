// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <memory>
#include <string>

/// Optional includes that are only required if DTLS is enabled.
#ifdef OCUDU_HAVE_OPENSSL_DTLS
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

namespace ocudu {

/// DTLS context interface used to abstract away OpenSSL specific details if
/// no openSSL with SCTP DTLS is present.
class dtls_context
{
public:
  virtual bool init(std::string session_id) = 0;
  virtual ~dtls_context()                   = default;
};

/// Creates an instance of a DTLS context.
std::unique_ptr<dtls_context> create_dtls_context();

#ifdef OCUDU_HAVE_OPENSSL_DTLS

constexpr bool OCUDU_DTLS_SCTP_SUPPORT = true;

class openssl_dtls_context : public dtls_context
{
public:
  openssl_dtls_context();
  ~openssl_dtls_context() override;
  bool init(std::string session_id) override;

private:
  ocudulog::basic_logger& logger;
  SSL_CTX*                ssl_ctx = nullptr;
};

#else
constexpr bool OCUDU_DTLS_SCTP_SUPPORT = false;
#endif
} // namespace ocudu
