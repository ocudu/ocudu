// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
#include "sctp_dtls_ssl.h"
#include "sctp_dtls.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include <linux/sctp.h>
#include <string>
#include <utility>

using namespace ocudu;

#ifdef OCUDU_HAVE_OPENSSL_DTLS

static std::string get_ssl_error_string(int err);

std::unique_ptr<dtls_ssl> ocudu::create_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_)
{
  /// Creates an instance of a DTLS context.
  return std::make_unique<openssl_dtls_ssl>(cfg_, deps_);
}

openssl_dtls_ssl::openssl_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_) :
  cfg(cfg_), ssl_ctx(deps_.ssl_ctx), logger(ocudulog::fetch_basic_logger("SCTP"))
{
}

openssl_dtls_ssl::~openssl_dtls_ssl()
{
  SSL_free(ssl);
}

bool openssl_dtls_ssl::init(int socket)
{
  /// Create SSL connection and BIO. We associate this BIO with the correct association at this point.
  SSL_CTX* ctx = static_cast<openssl_dtls_context&>(ssl_ctx).get_ssl_ctx();
  if (ctx == nullptr) {
    return false;
  }
  ssl = SSL_new(ctx);
  if (ssl == nullptr) {
    int err = ERR_get_error();
    logger.error("Could not initialize SSL. Cause: failure to create SSL. err={}", ERR_reason_error_string(err));
    return false;
  }
  bio = BIO_new_dgram_sctp(socket, BIO_NOCLOSE);
  if (bio == nullptr) {
    int err = ERR_get_error();
    logger.error("Could not initialize SSL. Cause: failure to create BIO. err={}", ERR_reason_error_string(err));
    return false;
  }
  SSL_set_bio(ssl, bio, bio);

  BIO_dgram_sctp_notification_handler_fn cb = &openssl_dtls_ssl::dtls_notification_cb;
  BIO_dgram_sctp_notification_cb(bio, cb, nullptr);

  // Initiate handshake.
  int ret = -1;
  if (cfg.mode == dtls_mode::server) {
    ret = SSL_accept(ssl);
  } else {
    ret = SSL_connect(ssl);
  }

  if (ret <= 0) {
    int err = SSL_get_error(ssl, ret);
    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
      logger.error(
          "DTLS {} failed. err={}", cfg.mode == dtls_mode::server ? "accept" : "connect", get_ssl_error_string(err));
      return false;
    }
  }
  logger.debug("DTLS handshake finished");
  return true;
}

bool openssl_dtls_ssl::is_init_finished()
{
  return SSL_is_init_finished(ssl);
}

bool openssl_dtls_ssl::handshake()
{
  if (SSL_is_init_finished(ssl)) {
    return false;
  }
  /// Do the handshake.
  int ret = -1;
  if (cfg.mode == dtls_mode::server) {
    ret = SSL_accept(ssl);
  } else {
    ret = SSL_connect(ssl);
  }
  return ret == 1;
}

expected<byte_buffer> openssl_dtls_ssl::receive()
{
  /// SSL should be initialized from here on.
  std::array<uint8_t, 9000> buff;
  int                       len = SSL_read(ssl, buff.data(), 9000);

  if (len <= 0) {
    int err = SSL_get_error(ssl, len);
    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
      char buf[256];
      ERR_error_string_n(err, buf, sizeof(buf));
      logger.error("SSL_read returned {}, SSL_get_error={} {}", len, err, get_ssl_error_string(err));
    }
    return make_unexpected(default_error_t{});
  }
  logger.debug("Read {} bytes from DTLS connection", len);
  auto buffer = byte_buffer{byte_buffer::fallback_allocation_tag{}, buff};
  return buffer;
}

int openssl_dtls_ssl::write(span<const uint8_t> pdu_span)
{
  return SSL_write(ssl, pdu_span.data(), pdu_span.size());
}

void openssl_dtls_ssl::dtls_notification_cb(BIO* bio, void* context, void* buf)
{
  // TODO handle notifications.
}

static std::string get_ssl_error_string(int err)
{
  std::string error_str;

  switch (err) {
    case SSL_ERROR_NONE:
      error_str = "SSL_ERROR_NONE";
      break;
    case SSL_ERROR_ZERO_RETURN:
      error_str = "SSL_ERROR_ZERO_RETURN";
      break;
    case SSL_ERROR_WANT_READ:
      error_str = "SSL_ERROR_WANT_READ";
      break;
    case SSL_ERROR_WANT_WRITE:
      error_str = "SSL_ERROR_WANT_WRITE";
      break;
    case SSL_ERROR_WANT_CONNECT:
      error_str = "SSL_ERROR_WANT_CONNECT";
      break;
    case SSL_ERROR_WANT_ACCEPT:
      error_str = "SSL_ERROR_WANT_ACCEPT";
      break;
    case SSL_ERROR_WANT_X509_LOOKUP:
      error_str = "SSL_ERROR_WANT_X509_LOOKUP";
      break;
    case SSL_ERROR_SYSCALL:
      error_str = "SSL_ERROR_SYSCALL";
      break;
    case SSL_ERROR_SSL:
      error_str = "SSL_ERROR_SSL";
      break;
    default:
      error_str = "UNKNOWN";
      break;
  }
  return error_str;
}

#else

std::unique_ptr<dtls_ssl> ocudu::create_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_)
{
  report_error("Trying to create DTLS SSL association, but DTLS is not supported");
  return nullptr;
}

#endif
