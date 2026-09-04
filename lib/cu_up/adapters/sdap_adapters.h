// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_tunnel_psup_tx.h"
#include "ocudu/pdcp/pdcp_tx.h"
#include "ocudu/sdap/sdap.h"

namespace ocudu {
namespace ocuup {

/// Adapter between SDAP and GTP-U
class sdap_gtpu_adapter : public sdap_rx_sdu_notifier
{
public:
  sdap_gtpu_adapter()  = default;
  ~sdap_gtpu_adapter() = default;

  void connect_gtpu(gtpu_tunnel_psup_tx_lower_layer_interface& gtpu_handler_) { gtpu_handler = &gtpu_handler_; }

  void on_new_sdu(byte_buffer sdu, qos_flow_id_t qfi) override
  {
    ocudu_assert(gtpu_handler != nullptr, "GTPU handler must not be nullptr");
    gtpu_handler->handle_sdu(std::move(sdu), qfi);
  }

private:
  gtpu_tunnel_psup_tx_lower_layer_interface* gtpu_handler = nullptr;
};

class sdap_pdcp_adapter : public sdap_tx_pdu_notifier
{
public:
  sdap_pdcp_adapter()  = default;
  ~sdap_pdcp_adapter() = default;

  void connect_pdcp(pdcp_tx_upper_data_interface& pdcp_handler_) { pdcp_handler = &pdcp_handler_; }
  void disconnect_pdcp() { pdcp_handler = nullptr; }

  void on_new_pdu(byte_buffer pdu) override
  {
    if (pdcp_handler == nullptr) {
      ocudulog::fetch_basic_logger("SDAP").warning("Unconnected PDCP handler. Dropping SDAP PDU");
    } else {
      pdcp_handler->handle_sdu(std::move(pdu));
    }
  }

private:
  pdcp_tx_upper_data_interface* pdcp_handler = nullptr;
};

} // namespace ocuup
} // namespace ocudu
