// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/f1ap/gateways/f1c_local_connector_factory.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/cu_cp_f1c_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/gateways/f1c_network_client_factory.h"
#include "ocudu/f1ap/gateways/f1c_network_server_factory.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

namespace {

/// Decorator for f1ap_message_notifier that writes the forwarded PDU to a pcap file.
class f1ap_pdu_pcap_notifier final : public f1ap_message_notifier
{
public:
  f1ap_pdu_pcap_notifier(std::unique_ptr<f1ap_message_notifier> decorated_,
                         dlt_pcap&                              pcap_writer_,
                         ocudulog::basic_logger&                logger_) :
    logger(logger_), pcap_writer(pcap_writer_), decorated(std::move(decorated_))
  {
    ocudu_sanity_check(pcap_writer.is_write_enabled(), "Pcap writing must be enabled.");
  }

  void on_new_message(const f1ap_message& msg) override
  {
    byte_buffer   buf;
    asn1::bit_ref bref(buf);
    if (msg.pdu.pack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Failed to write Rx PDU to PCAP. Cause: Failed to pack Rx PDU");
    } else {
      pcap_writer.push_pdu(std::move(buf));
    }

    // Forward message to decorated class.
    decorated->on_new_message(msg);
  }

private:
  ocudulog::basic_logger&                logger;
  dlt_pcap&                              pcap_writer;
  std::unique_ptr<f1ap_message_notifier> decorated;
};

/// Implementation of a DU and CU-CP F1-C gateway for the case that the DU and CU-CP are co-located.
class f1c_local_connector_impl final : public f1c_local_connector
{
public:
  f1c_local_connector_impl(const f1c_local_connector_config& cfg) : pcap_writer(cfg.pcap) {}

  void attach_cu_cp(ocucp::cu_cp_f1c_handler& cu_cp_du_mng_) override { cu_cp_du_mng = &cu_cp_du_mng_; }

  void stop() override {}

  std::optional<uint16_t> get_listen_port() const override { return std::nullopt; }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_notifier) override
  {
    report_fatal_error_if_not(cu_cp_du_mng != nullptr, "CU-CP has not been attached to F1-C gateway.");

    // Decorate DU RX notifier with pcap writing.
    if (pcap_writer.is_write_enabled()) {
      du_notifier = std::make_unique<f1ap_pdu_pcap_notifier>(
          std::move(du_notifier), pcap_writer, ocudulog::fetch_basic_logger("DU-F1"));
    }

    // Create direct connection between CU-CP and DU notifier.
    auto cu_notifier = cu_cp_du_mng->handle_new_du_connection(std::move(du_notifier));
    if (cu_notifier == nullptr) {
      return nullptr;
    }

    // Decorate CU-CP RX notifier with pcap writing.
    if (pcap_writer.is_write_enabled()) {
      cu_notifier = std::make_unique<f1ap_pdu_pcap_notifier>(
          std::move(cu_notifier), pcap_writer, ocudulog::fetch_basic_logger("CU-CP-F1"));
    }

    return cu_notifier;
  }

private:
  dlt_pcap&                 pcap_writer;
  ocucp::cu_cp_f1c_handler* cu_cp_du_mng = nullptr;
};

/// Implementation of a DU and CU-CP F1-C SCTP-based gateway for the case that the DU and CU-CP are co-located.
///
/// Note: This class should only be used for testing purposes.
class f1c_sctp_connector_impl final : public f1c_local_connector
{
public:
  f1c_sctp_connector_impl(const f1c_local_sctp_connector_config& cfg) :
    broker(cfg.broker), io_rx_executor(cfg.io_rx_executor), pcap_writer(cfg.pcap)
  {
    // Create SCTP server.
    sctp_network_gateway_config sctp;
    sctp.if_name        = "F1-C";
    sctp.ppid           = F1AP_PPID;
    sctp.bind_addresses = {"127.0.0.1"};
    // Use any bind port available.
    sctp.bind_port = 0;
    server         = create_f1c_gateway_server(
        f1c_cu_sctp_gateway_config{sctp, broker, cfg.io_rx_executor, cfg.ctrl_exec, pcap_writer});
  }

  void stop() override { server->stop(); }

  void attach_cu_cp(ocucp::cu_cp_f1c_handler& cu_f1c_handler_) override
  {
    server->attach_cu_cp(cu_f1c_handler_);

    // Create SCTP client.
    sctp_network_connector_config sctp_client;
    sctp_client.if_name           = "F1-C";
    sctp_client.dest_name         = "CU-CP";
    sctp_client.connect_addresses = {"127.0.0.1"};
    sctp_client.connect_port      = server->get_listen_port().value();
    sctp_client.ppid              = F1AP_PPID;
    // Note: We only need to save the PCAPs in one side of the connection.
    client =
        create_f1c_gateway_client(f1c_du_sctp_gateway_config{sctp_client, broker, io_rx_executor, *null_pcap_writer});
  }

  std::optional<uint16_t> get_listen_port() const override { return server->get_listen_port(); }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override
  {
    // Connect client to server automatically.
    return client->handle_du_connection_request(std::move(du_rx_pdu_notifier));
  }

private:
  io_broker&                                    broker;
  task_executor&                                io_rx_executor;
  dlt_pcap&                                     pcap_writer;
  std::unique_ptr<dlt_pcap>                     null_pcap_writer = create_null_dlt_pcap();
  std::unique_ptr<ocucp::f1c_connection_server> server;
  std::unique_ptr<odu::f1c_connection_client>   client;
};

} // namespace

std::unique_ptr<f1c_local_connector> ocudu::create_f1c_local_connector(const f1c_local_connector_config& cfg)
{
  return std::make_unique<f1c_local_connector_impl>(cfg);
}

std::unique_ptr<f1c_local_connector> ocudu::create_f1c_local_connector(const f1c_local_sctp_connector_config& cfg)
{
  return std::make_unique<f1c_sctp_connector_impl>(cfg);
}
