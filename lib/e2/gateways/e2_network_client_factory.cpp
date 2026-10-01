// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/gateways/e2_network_client_factory.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2_messages.h"
#include "ocudu/gateways/sctp_network_client_factory.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/io/io_broker.h"

using namespace ocudu;

namespace {

/// Notifier for converting packed E2AP PDUs coming from the E2 GW into unpacked E2AP PDUs and forward them to the E2
/// Agent.
class sctp_to_e2_pdu_notifier final : public sctp_association_sdu_notifier
{
public:
  sctp_to_e2_pdu_notifier(std::unique_ptr<e2_message_notifier> du_rx_pdu_notifier_,
                          dlt_pcap&                            pcap_writer_,
                          ocudulog::basic_logger&              logger_) :
    e2_rx_pdu_notifier(std::move(du_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  bool on_new_sdu(byte_buffer sdu) override
  {
    // Unpack E2AP PDU.
    asn1::cbit_ref bref(sdu);
    e2_message     msg;
    if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Couldn't unpack E2AP PDU");
      return false;
    }

    // Forward Rx PDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(sdu.copy());
    }

    // Forward unpacked Rx PDU to the DU.
    e2_rx_pdu_notifier->on_new_message(msg);

    return true;
  }

private:
  std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier;
  dlt_pcap&                            pcap_writer;
  ocudulog::basic_logger&              logger;
};

/// Notifier for converting unpacked E2AP PDUs coming from the E2 Agent into packed E2AP PDUs and forward them to the
/// E2AP-GW.
class e2_to_sctp_pdu_notifier final : public e2_message_notifier
{
public:
  e2_to_sctp_pdu_notifier(std::unique_ptr<sctp_association_sdu_notifier> sctp_rx_pdu_notifier_,
                          dlt_pcap&                                      pcap_writer_,
                          ocudulog::basic_logger&                        logger_) :
    sctp_rx_pdu_notifier(std::move(sctp_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  void on_new_message(const e2_message& msg) override
  {
    // pack E2AP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Failed to pack E2AP PDU");
      return;
    }

    // Push Tx PDU to pcap.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(tx_sdu.copy());
    }

    // Forward packed Tx PDU to SCTP gateway.
    sctp_rx_pdu_notifier->on_new_sdu(std::move(tx_sdu));
  }

private:
  std::unique_ptr<sctp_association_sdu_notifier> sctp_rx_pdu_notifier;
  dlt_pcap&                                      pcap_writer;
  ocudulog::basic_logger&                        logger;
};

class e2_sctp_gateway_client final : public e2_connection_client
{
public:
  e2_sctp_gateway_client(const e2_sctp_gateway_config& params) :
    pcap_writer(params.pcap), broker(params.broker), sctp_params(params.sctp), logger(params.logger)
  {
    // Create SCTP network adapter.
    sctp_gateway = create_sctp_network_client(sctp_network_client_config{params.sctp, broker, params.io_rx_executor});
    report_error_if_not(sctp_gateway != nullptr, "Failed to create SCTP gateway client.\n");
  }

  std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier) override
  {
    ocudu_assert(e2_rx_pdu_notifier != nullptr, "E2 Agent Rx PDU notifier is null");

    logger.debug("Trying to establish E2 connection to Near-RT RIC (configured addrs {}, port {})...",
                 fmt::join(sctp_params.connect_addresses, ","),
                 sctp_params.connect_port);
    std::unique_ptr<sctp_association_sdu_notifier> sctp_sender = sctp_gateway->connect(
        std::make_unique<sctp_to_e2_pdu_notifier>(std::move(e2_rx_pdu_notifier), pcap_writer, logger));
    if (sctp_sender == nullptr) {
      logger.error("Failed to establish E2 connection to Near-RT RIC (configured addrs {}, port {}).",
                   fmt::join(sctp_params.connect_addresses, ","),
                   sctp_params.connect_port);
      return nullptr;
    }
    logger.info("{}: Connection to {} on {}:{} established",
                sctp_params.if_name,
                sctp_params.dest_name,
                fmt::join(sctp_params.connect_addresses, ","),
                sctp_params.connect_port);
    fmt::print("{}: Connection to {} on {}:{} established\n",
               sctp_params.if_name,
               sctp_params.dest_name,
               fmt::join(sctp_params.connect_addresses, ","),
               sctp_params.connect_port);

    // Return the Tx PDU notifier to the DU.
    return std::make_unique<e2_to_sctp_pdu_notifier>(std::move(sctp_sender), pcap_writer, logger);
  }

private:
  dlt_pcap&                            pcap_writer;
  io_broker&                           broker;
  ocudu::sctp_network_connector_config sctp_params;
  ocudulog::basic_logger&              logger;

  // SCTP network gateway
  std::unique_ptr<sctp_network_client> sctp_gateway;
};

} // namespace

std::unique_ptr<e2_connection_client> ocudu::create_e2_gateway_client(const e2_sctp_gateway_config& params)
{
  return std::make_unique<e2_sctp_gateway_client>(params);
}
