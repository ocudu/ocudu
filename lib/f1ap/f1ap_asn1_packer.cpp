// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_asn1_packer.h"
#include "ocudu/f1ap/f1ap_message.h"

using namespace ocudu;

f1ap_asn1_packer::f1ap_asn1_packer(sctp_network_gateway_data_handler& gw_, f1ap_message_handler& f1ap_handler) :
  logger(ocudulog::fetch_basic_logger("F1AP-ASN1-PCK")), gw(gw_), f1ap(f1ap_handler)
{
}

// Received packed F1AP PDU that needs to be unpacked and forwarded.
void f1ap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  logger.debug("Received PDU of {} bytes", bytes.length());

  asn1::cbit_ref bref(bytes);
  f1ap_message   msg = {};
  if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.error("Couldn't unpack PDU");
    return;
  }

  // call packet handler
  f1ap.handle_message(msg);
}

// Receive populated ASN1 struct that needs to be packed and forwarded.
void f1ap_asn1_packer::handle_message(const f1ap_message& msg)
{
  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (msg.pdu.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.error("Failed to pack PDU");
    return;
  }

  gw.handle_pdu(tx_pdu);
}
