// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/gateways/sctp_dtls_mode.h"
#include "ocudu/ocudulog/logger.h"
#include <memory>
#include <netinet/in.h>
#include <netinet/sctp.h>

/// Optional includes that are only required if DTLS is enabled.
#ifdef OCUDU_HAVE_OPENSSL_DTLS
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

namespace ocudu {

struct dtls_ssl_config {
  dtls_mode    mode;
  sctp_assoc_t assoc;
};

enum class dtls_ssl_read_error { shutdown, unknown };

class dtls_context;
class sctp_network_gateway_dtls_interface;

struct dtls_ssl_dependencies {
  dtls_context&                        ssl_ctx;
  sctp_network_gateway_dtls_interface& gw;
};

/// DTLS context interface used to abstract away OpenSSL specific details of
/// the SSL session.
class dtls_ssl
{
public:
  virtual bool                                       init(int socket)                    = 0;
  virtual bool                                       shutdown()                          = 0;
  virtual bool                                       is_init_finished()                  = 0;
  virtual bool                                       handshake()                         = 0;
  virtual expected<byte_buffer, dtls_ssl_read_error> receive()                           = 0;
  virtual int                                        write(span<const uint8_t> pdu_span) = 0;
  virtual ~dtls_ssl()                                                                    = default;
};

/// Creates an instance of a DTLS context.
std::unique_ptr<dtls_ssl> create_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& deps_);

#ifdef OCUDU_HAVE_OPENSSL_DTLS

/// DTLS SSL wrapper used to implement the SSL handshake, wrap the BIO and SSL
/// openSSL obejcts, etc.
class openssl_dtls_ssl : public dtls_ssl
{
public:
  openssl_dtls_ssl(const dtls_ssl_config& cfg_, const dtls_ssl_dependencies& ssl_ctx_);
  ~openssl_dtls_ssl() override;
  bool                                       init(int socket) override;
  bool                                       shutdown() override;
  bool                                       is_init_finished() override;
  bool                                       handshake() override;
  expected<byte_buffer, dtls_ssl_read_error> receive() override;
  int                                        write(span<const uint8_t> pdu_span) override;

private:
  static void dtls_notification_cb(BIO* bio, void* context, void* buf);
  void        send_test_data(int line);

  dtls_ssl_config cfg;
  BIO*            bio = nullptr;
  SSL*            ssl = nullptr;

  dtls_context&                        ssl_ctx;
  sctp_network_gateway_dtls_interface& gw;
  static constexpr uint32_t            dtls_max_len = 9100;

  ocudulog::basic_logger& logger;
};

#endif

} // namespace ocudu
