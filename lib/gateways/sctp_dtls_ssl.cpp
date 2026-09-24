// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
#include "sctp_dtls_ssl.h"
#include "openssl_error.h"
#include "sctp_dtls.h"
#include "sctp_network_gateway_dtls_interface.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include <string>

using namespace ocudu;

#ifdef OCUDU_HAVE_OPENSSL_DTLS

static std::string get_ssl_error_string(int err);

std::unique_ptr<dtls_ssl> ocudu::create_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_)
{
  /// Creates an instance of a DTLS context.
  return std::make_unique<openssl_dtls_ssl>(cfg_, deps_);
}

openssl_dtls_ssl::openssl_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_) :
  cfg(cfg_), ssl_ctx(deps_.ssl_ctx), gw(deps_.gw), logger(ocudulog::fetch_basic_logger("SCTP"))
{
  logger.info("DTLS session created. mode={} assoc={}", format_as(cfg.mode), cfg.assoc);
}

openssl_dtls_ssl::~openssl_dtls_ssl()
{
  SSL_free(ssl);
}

bool openssl_dtls_ssl::init(int socket)
{
  socket_ = socket;
  /// Create SSL connection and BIO. We associate this BIO with the correct association at this point.
  SSL_CTX* ctx = static_cast<openssl_dtls_context&>(ssl_ctx).get_ssl_ctx();
  if (ctx == nullptr) {
    return false;
  }
  ssl = SSL_new(ctx);
  if (ssl == nullptr) {
    logger.error("Could not initialize DTLS session. Cause: failure to create SSL. err={}",
                 openssl_error{ERR_get_error()});
    return false;
  }

  bio = BIO_new_dgram_sctp(socket, BIO_NOCLOSE);
  if (bio == nullptr) {
    logger.error("Could not initialize DTLS session. Cause: failure to create BIO. err={}",
                 openssl_error{ERR_get_error()});
    return false;
  }

  SSL_set_bio(ssl, bio, bio);
  BIO_dgram_sctp_notification_handler_fn cb = &openssl_dtls_ssl::dtls_notification_cb;
  BIO_dgram_sctp_notification_cb(bio, cb, this);

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
  logger.debug("DTLS SSL session initialized");
  return true;
}

bool openssl_dtls_ssl::shutdown()
{
  int ret = SSL_shutdown(ssl);
  if (ret == 1) {
    logger.debug("SSL_shutdown success: ret={}", ret);
    return true;
  }

  if (ret == 0) {
    // Our close_notify was sent.
    // We're intentionally not waiting for the peer's.
    logger.debug("SSL_shutdown success: ret={}", ret);
    return true;
  }

  int err = SSL_get_error(ssl, ret);

  logger.error("SSL_shutdown failed: ret={}, ssl_error={}", ret, err);
  return false;
}

bool openssl_dtls_ssl::is_init_finished()
{
  return SSL_is_init_finished(ssl);
}

bool openssl_dtls_ssl::handshake()
{
  if (SSL_is_init_finished(ssl)) {
    logger.debug("DTLS SSL handshake already done");
    return false;
  }

  /// Do the handshake.
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

  if (ret != 1) {
    logger.debug("DTLS SSL handshake on going. mode={} state={}",
                 cfg.mode == dtls_mode::server ? "server" : "client",
                 SSL_state_string_long(ssl));
  } else {
    logger.debug("DTLS SSL handshake finished. mode={} state={}",
                 cfg.mode == dtls_mode::server ? "server" : "client",
                 SSL_state_string_long(ssl));
  }
  return ret == 1;
}

expected<byte_buffer> openssl_dtls_ssl::receive()
{
  /// SSL should be initialized from here on.
  std::array<uint8_t, dtls_max_len> buff;
  int                               ret = SSL_read(ssl, buff.data(), dtls_max_len);

  if (ret <= 0) {
    unsigned long ssl_error = SSL_get_error(ssl, ret);
    if (ssl_error == SSL_ERROR_ZERO_RETURN) {
      logger.error("SSL_read returned SSL_ERROR_ZERO_RETURN, SSL_get_error={}", openssl_error{ssl_error});
      SSL_shutdown(ssl);
      return make_unexpected(default_error_t{});
    }
    logger.error("SSL_read returned {}, SSL_get_error={}", ret, ssl_error);
    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
      char error_buf[256];
      ERR_error_string_n(err, error_buf, sizeof(error_buf));
      logger.error("OpenSSL error: {}", error_buf);
    }
    return make_unexpected(default_error_t{});
  }

  logger.debug("Read {} bytes from DTLS connection", ret);
  auto buffer =
      byte_buffer{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(buff.begin(), buff.begin() + ret)};
  return buffer;
}

int openssl_dtls_ssl::write(span<const uint8_t> pdu_span)
{
  int bytes_written = SSL_write(ssl, pdu_span.data(), pdu_span.size());
  if (bytes_written <= 0) {
    int err = SSL_get_error(ssl, bytes_written);
    logger.error("Could not write {} bytes to DTLS connection. err={}", pdu_span.size(), get_ssl_error_string(err));
  }
  return bytes_written;
}

void openssl_dtls_ssl::dtls_notification_cb(BIO* bio, void* context, void* buf)
{
  // Get SSL context from notification.
  auto*       ssl   = static_cast<openssl_dtls_ssl*>(context);
  const auto* notif = static_cast<const union sctp_notification*>(buf);
  ssl->gw.handle_dtls_notification(notif, ssl->cfg.assoc);
}

void openssl_dtls_ssl::send_test_data(int line)
{
  char test = 'X';

  errno     = 0;
  ssize_t n = send(socket_, &test, 1, MSG_NOSIGNAL);

  int saved_errno = errno;

  logger.error("SCTP test send: n={} errno={} ({}), line={}", n, saved_errno, strerror(saved_errno), line);
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
