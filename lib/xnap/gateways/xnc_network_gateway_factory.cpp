// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/xnap/gateways/xnc_network_gateway_factory.h"
#include "ocudu/asn1/xnap/xnap.h"
#include "ocudu/cu_cp/cu_cp_xnc_handler.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/io/transport_layer_address.h"
#include "ocudu/xnap/xnap_message.h"
#include <cstdint>

using namespace ocudu;
using namespace ocudu::ocucp;

namespace {

/// Notifier passed to the CU-CP, which the CU-CP will use to send XNAP Tx PDUs.
class xnc_to_gw_pdu_notifier final : public xnap_message_notifier
{
public:
  xnc_to_gw_pdu_notifier(std::unique_ptr<sctp_association_sdu_notifier> sctp_sender_,
                         dlt_pcap&                                      pcap_writer_,
                         ocudulog::basic_logger&                        logger_) :
    sctp_sender(std::move(sctp_sender_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  /// Handle unpacked Tx XNAP PDU by packing and forwarding it into the SCTP GW.
  bool on_new_message(const xnap_message& msg) override
  {
    // Pack XNAP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Failed to pack XNAP PDU");
      return false;
    }

    // Push Tx PDU to pcap.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(tx_sdu.copy());
    }

    // Forward packed XNAP Tx PDU to SCTP gateway.
    sctp_sender->on_new_sdu(std::move(tx_sdu));
    return true;
  }

private:
  std::unique_ptr<sctp_association_sdu_notifier> sctp_sender;
  dlt_pcap&                                      pcap_writer;
  ocudulog::basic_logger&                        logger;
};

/// Notifier passed to the SCTP GW, which the GW will use to forward XNAP Rx PDUs to the CU-CP.
class gw_to_xnc_pdu_notifier final : public sctp_association_sdu_notifier
{
public:
  gw_to_xnc_pdu_notifier(std::unique_ptr<xnap_message_notifier> xnap_notifier_,
                         dlt_pcap&                              pcap_writer_,
                         ocudulog::basic_logger&                logger_) :
    xnap_notifier(std::move(xnap_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  bool on_new_sdu(byte_buffer sdu) override
  {
    // Unpack SCTP SDU into XNAP PDU.
    asn1::cbit_ref bref(sdu);
    xnap_message   msg;
    if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Couldn't unpack XNAP PDU");
      return false;
    }

    // Forward SCTP Rx SDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(sdu.copy());
    }

    // Forward unpacked Rx PDU to the CU-CP.
    if (!xnap_notifier->on_new_message(msg)) {
      return false;
    }

    return true;
  }

private:
  std::unique_ptr<xnap_message_notifier> xnap_notifier;
  dlt_pcap&                              pcap_writer;
  ocudulog::basic_logger&                logger;
};

/// Adapter of the SCTP server to the XN-C interface of the CU-CP.
class xnc_sctp_server final : public ocucp::xnc_connection_gateway, public sctp_network_association_factory
{
public:
  xnc_sctp_server(const xnc_sctp_gateway_config& params_) : params(params_)
  {
    // Create SCTP server.
    sctp_server = create_sctp_network_server(
        sctp_network_server_config{params.sctp, params.broker, params.io_rx_executor, params.ctrl_exec, *this});
    report_error_if_not(sctp_server != nullptr, "Failed to create SCTP server");
  }

  void stop() override { sctp_server->stop(); }

  void attach_cu_cp(ocucp::cu_cp_xnc_handler& xnc_handler_) override
  {
    xnc_handler = &xnc_handler_;

    // Start listening for new CU-CP SCTP connections.
    bool result = sctp_server->listen();
    report_error_if_not(result, "Failed to start SCTP server.\n");
    fmt::print("{}: Listening for new connections on bind addresses {}, port {}...\n",
               params.sctp.if_name,
               fmt::join(params.sctp.bind_addresses, ","),
               params.sctp.bind_port);
  }

  async_task<bool> connect_to_peer(transport_layer_address peer_addr) override
  {
    return launch_async([this, peer_addr, result = false](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(result, sctp_server->connect({peer_addr}));
      CORO_RETURN(result);
    });
  }

  std::optional<uint16_t> get_listen_port() const override { return sctp_server->get_listen_port(); }

  std::unique_ptr<sctp_association_sdu_notifier>
  create(std::unique_ptr<sctp_association_sdu_notifier> sctp_send_notifier, sctp_association_info assoc_info) override
  {
    // Create an unpacked XNAP PDU notifier and pass it to the CU-CP.
    auto xnc_sender = std::make_unique<xnc_to_gw_pdu_notifier>(std::move(sctp_send_notifier), params.pcap, logger);

    std::unique_ptr<xnap_message_notifier> xnc_receiver =
        xnc_handler->handle_new_xnc_cu_cp_connection(std::move(xnc_sender), assoc_info);

    // Wrap the received XNAP Rx PDU notifier in an SCTP notifier and return it.
    if (xnc_receiver == nullptr) {
      return nullptr;
    }

    return std::make_unique<gw_to_xnc_pdu_notifier>(std::move(xnc_receiver), params.pcap, logger);
  }

private:
  const xnc_sctp_gateway_config params;
  ocudulog::basic_logger&       logger      = ocudulog::fetch_basic_logger("CU-CP-XN");
  ocucp::cu_cp_xnc_handler*     xnc_handler = nullptr;

  std::unique_ptr<sctp_network_server> sctp_server;
};

} // namespace

std::unique_ptr<ocucp::xnc_connection_gateway> ocudu::create_xnc_connection_gateway(const xnc_sctp_gateway_config& cfg)
{
  return std::make_unique<xnc_sctp_server>(cfg);
}
