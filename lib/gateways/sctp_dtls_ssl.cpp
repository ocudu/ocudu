// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
#include "sctp_dtls_ssl.h"
#include "sctp_dtls.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include <linux/sctp.h>
#include <string>
#include <utility>

using namespace ocudu;

#ifdef OCUDU_HAVE_OPENSSL_DTLS

static std::string get_ssl_error_string(int err);
static const char* sctp_notification_type_name(uint16_t type);

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
      logger.error("connect/accept failed. err={}", err);
      fmt::println("connect/accept failed. err={}", err);
      return false;
    }
  }
  fmt::println(stderr, "handshake finished");
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

bool openssl_dtls_ssl::receive()
{
  /// SSL should be initialized from here on.
  std::array<uint8_t, 9000> buff;
  int                       len = SSL_read(ssl, buff.data(), 9000);

  if (len <= 0) {
    int  err = SSL_get_error(ssl, len);
    char buf[256];
    ERR_error_string_n(err, buf, sizeof(buf));
    fprintf(stderr, "SSL_read returned %d, SSL_get_error=%d %s\n", len, err, get_ssl_error_string(err).c_str());
    return false;
  }
  fmt::println("Read {} bytes from SSL", len);
  return true;
}

void openssl_dtls_ssl::dtls_notification_cb(BIO* bio, void* context, void* buf)
{
  if (buf == nullptr) {
    fmt::println(stderr, "got SCTP notification: <null>");
    return;
  }

  auto* sn = static_cast<sctp_notification*>(buf);

  fmt::println(stderr,
               "got SCTP notification: type={} ({})",
               sn->sn_header.sn_type,
               sctp_notification_type_name(sn->sn_header.sn_type));
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

const char* sctp_notification_type_name(uint16_t type)
{
  switch (type) {
    case SCTP_ASSOC_CHANGE:
      return "SCTP_ASSOC_CHANGE";
    case SCTP_PEER_ADDR_CHANGE:
      return "SCTP_PEER_ADDR_CHANGE";
    case SCTP_REMOTE_ERROR:
      return "SCTP_REMOTE_ERROR";
    case SCTP_SEND_FAILED:
      return "SCTP_SEND_FAILED";
    case SCTP_SHUTDOWN_EVENT:
      return "SCTP_SHUTDOWN_EVENT";
    case SCTP_ADAPTATION_INDICATION:
      return "SCTP_ADAPTATION_INDICATION";
    case SCTP_PARTIAL_DELIVERY_EVENT:
      return "SCTP_PARTIAL_DELIVERY_EVENT";
    case SCTP_AUTHENTICATION_EVENT:
      return "SCTP_AUTHENTICATION_EVENT";
    case SCTP_SENDER_DRY_EVENT:
      return "SCTP_SENDER_DRY_EVENT";
    case SCTP_STREAM_RESET_EVENT:
      return "SCTP_STREAM_RESET_EVENT";
    case SCTP_ASSOC_RESET_EVENT:
      return "SCTP_ASSOC_RESET_EVENT";
    case SCTP_STREAM_CHANGE_EVENT:
      return "SCTP_STREAM_CHANGE_EVENT";
    default:
      return "UNKNOWN";
  }
}
#else

std::unique_ptr<dtls_ssl> ocudu::create_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_)
{
  report_error("Trying to create DTLS SSL association, but DTLS is not supported");
  return nullptr;
}

#endif
