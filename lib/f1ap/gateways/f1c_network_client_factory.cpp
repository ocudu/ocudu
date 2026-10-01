// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/f1ap/gateways/f1c_network_client_factory.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/gateways/sctp_network_client_factory.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/io/io_broker.h"

using namespace ocudu;

namespace {

/// Notifier for converting packed F1AP PDUs coming from the F1-C GW into unpacked F1AP PDUs and forward them to the DU.
class sctp_to_f1c_pdu_notifier final : public sctp_association_sdu_notifier
{
public:
  sctp_to_f1c_pdu_notifier(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier_,
                           dlt_pcap&                              pcap_writer_,
                           ocudulog::basic_logger&                logger_) :
    du_rx_pdu_notifier(std::move(du_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  bool on_new_sdu(byte_buffer sdu) override
  {
    // Unpack F1AP PDU.
    asn1::cbit_ref bref(sdu);
    f1ap_message   msg;
    if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Couldn't unpack F1AP PDU");
      return false;
    }

    // Forward Rx PDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(sdu.copy());
    }

    // Forward unpacked Rx PDU to the DU.
    du_rx_pdu_notifier->on_new_message(msg);

    return true;
  }

private:
  std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier;
  dlt_pcap&                              pcap_writer;
  ocudulog::basic_logger&                logger;
};

/// Notifier for converting unpacked F1AP PDUs coming from the DU into packed F1AP PDUs and forward them to the F1C-GW.
class f1c_to_sctp_pdu_notifier final : public f1ap_message_notifier
{
public:
  f1c_to_sctp_pdu_notifier(std::unique_ptr<sctp_association_sdu_notifier> sctp_rx_pdu_notifier_,
                           dlt_pcap&                                      pcap_writer_,
                           ocudulog::basic_logger&                        logger_) :
    sctp_rx_pdu_notifier(std::move(sctp_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    // pack F1AP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Failed to pack F1AP PDU");
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

class f1c_sctp_gateway_client final : public odu::f1c_connection_client
{
public:
  f1c_sctp_gateway_client(const f1c_du_sctp_gateway_config& params) :
    pcap_writer(params.pcap), broker(params.broker), sctp_params(params.sctp)
  {
    // Create SCTP network adapter.
    sctp_gateway = create_sctp_network_client(sctp_network_client_config{params.sctp, broker, params.io_rx_executor});
    report_error_if_not(sctp_gateway != nullptr, "Failed to create SCTP gateway client.\n");
  }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override
  {
    ocudu_assert(du_rx_pdu_notifier != nullptr, "DU Rx PDU notifier is null");

    logger.debug(
        "Establishing TNL connection to CU-CP ({}:{})...", sctp_params.connect_addresses[0], sctp_params.connect_port);
    std::unique_ptr<sctp_association_sdu_notifier> sctp_sender = sctp_gateway->connect(
        std::make_unique<sctp_to_f1c_pdu_notifier>(std::move(du_rx_pdu_notifier), pcap_writer, logger));
    if (sctp_sender == nullptr) {
      // Note: Logged at debug level, as the SCTP gateway already reports the failure with its cause, and throttles the
      // repetitions when the connection is retried.
      logger.debug("Failed to establish F1-C TNL connection to CU-CP on {}:{}.",
                   sctp_params.connect_addresses[0],
                   sctp_params.connect_port);
      return nullptr;
    }
    logger.info("{}: TNL connection to {} on {}:{} accepted",
                sctp_params.if_name,
                sctp_params.dest_name,
                sctp_params.connect_addresses[0],
                sctp_params.connect_port);
    fmt::print("{}: Connection to {} on {}:{} completed\n",
               sctp_params.if_name,
               sctp_params.dest_name,
               sctp_params.connect_addresses[0],
               sctp_params.connect_port);

    // Return the Tx PDU notifier to the DU.
    return std::make_unique<f1c_to_sctp_pdu_notifier>(std::move(sctp_sender), pcap_writer, logger);
  }

private:
  dlt_pcap&                            pcap_writer;
  io_broker&                           broker;
  ocudu::sctp_network_connector_config sctp_params;
  ocudulog::basic_logger&              logger = ocudulog::fetch_basic_logger("DU-F1");

  // SCTP network gateway
  std::unique_ptr<sctp_network_client> sctp_gateway;
};

} // namespace

std::unique_ptr<odu::f1c_connection_client> ocudu::create_f1c_gateway_client(const f1c_du_sctp_gateway_config& params)
{
  return std::make_unique<f1c_sctp_gateway_client>(params);
}
