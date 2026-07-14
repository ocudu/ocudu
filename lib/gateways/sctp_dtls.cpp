// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
#include "sctp_dtls.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include <string>

using namespace ocudu;

#ifdef OCUDU_HAVE_OPENSSL_DTLS

/// Creates an instance of a DTLS context.
std::unique_ptr<dtls_context> ocudu::create_dtls_context()
{
  return std::make_unique<openssl_dtls_context>();
}

/// DTLS context instance
openssl_dtls_context::openssl_dtls_context() : logger(ocudulog::fetch_basic_logger("SCTP")) {}

openssl_dtls_context::~openssl_dtls_context()
{
  if (ssl_ctx != nullptr) {
    SSL_CTX_free(ssl_ctx);
  }
}

bool openssl_dtls_context::init(std::string session_id)
{
  // TODO
  ssl_ctx = SSL_CTX_new(DTLS_server_method());
  if (ssl_ctx == nullptr) {
    unsigned long err = ERR_get_error();
    logger.error("Could not initialize DTLS context. session={} err={}", session_id, ERR_reason_error_string(err));
    return false;
  }
  return true;
}

#else

std::unique_ptr<dtls_context> ocudu::create_dtls_context()
{
  report_error("Trying to create DTLS context, but DTLS is not supported");
  return nullptr;
}

#endif
