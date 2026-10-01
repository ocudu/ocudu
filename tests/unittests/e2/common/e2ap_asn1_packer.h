// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/e2/e2.h"
#include "ocudu/gateways/sctp_network_client.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/pcap/dlt_pcap.h"

namespace ocudu {

/// This E2AP packer class is used to pack outgoing and unpack incoming E2 message in ASN1 format.
class e2ap_asn1_packer
{
public:
  explicit e2ap_asn1_packer(sctp_association_sdu_notifier& gw, e2_message_handler& e2, dlt_pcap& pcap_);

  /// Received packed E2AP PDU that needs to be unpacked and forwarded.
  void handle_packed_pdu(const byte_buffer& pdu);

  /// Receive populated ASN1 struct that needs to be packed and forwarded.
  void handle_message(const e2_message& msg);

private:
  ocudulog::basic_logger&        logger;
  sctp_association_sdu_notifier& gw;
  e2_message_handler&            e2;
  dlt_pcap&                      pcap;
};

} // namespace ocudu
