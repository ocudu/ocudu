// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_gateway.h"
#include "ocudu/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "ocudu/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "ocudu/pdcp/pdcp_rx.h"

namespace ocudu::ocuup {

/// Adapter between F1-U and PDCP.
class f1u_pdcp_adapter final : public f1u_rx_sdu_notifier, public f1u_rx_delivery_notifier
{
public:
  void connect_pdcp(pdcp_rx_lower_interface& pdcp_rx_handler_, pdcp_tx_lower_interface& pdcp_tx_handler_)
  {
    pdcp_rx_handler = &pdcp_rx_handler_;
    pdcp_tx_handler = &pdcp_tx_handler_;
  }

  // See interface for documentation.
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    if (pdcp_rx_handler == nullptr) {
      ocudulog::fetch_basic_logger("F1-U").warning("Unconnected PDCP handler. Dropping F1-U SDU");
      return;
    }

    pdcp_rx_handler->handle_pdu(std::move(sdu));
  }

  // See interface for documentation.
  void on_desired_buffer_size_notification(uint32_t desired_buffer_size) override
  {
    pdcp_tx_handler->handle_desired_buffer_size_notification(desired_buffer_size);
  }

  // See interface for documentation.
  void on_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_transmit_notification(highest_pdcp_sn);
  }

  // See interface for documentation.
  void on_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_delivery_notification(highest_pdcp_sn);
  }

  // See interface for documentation.
  void on_retransmit_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_retransmit_notification(highest_pdcp_sn);
  }

  // See interface for documentation.
  void on_delivery_retransmitted_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_delivery_retransmitted_notification(highest_pdcp_sn);
  }

private:
  pdcp_rx_lower_interface* pdcp_rx_handler = nullptr;
  pdcp_tx_lower_interface* pdcp_tx_handler = nullptr;
};

/// Adapter between NR-U and F1-U gateway adapter
class f1u_gateway_rx_nru_adapter : public f1u_cu_up_gateway_bearer_rx_notifier
{
public:
  f1u_gateway_rx_nru_adapter()           = default;
  ~f1u_gateway_rx_nru_adapter() override = default;

  void connect_nru_bearer(f1u_rx_pdu_handler& f1u_handler_) { f1u_handler = &f1u_handler_; }

  // See interface for documentation.
  void on_new_pdu(nru_ul_message msg) override
  {
    ocudu_assert(f1u_handler != nullptr, "GTP-U handler must not be nullptr");
    if (f1u_handler == nullptr) {
      ocudulog::fetch_basic_logger("F1-U").warning("Unconnected F1U handler. Dropping F1-U PDU");
      return;
    }

    f1u_handler->handle_pdu(std::move(msg));
  }

private:
  f1u_rx_pdu_handler* f1u_handler = nullptr;
};

} // namespace ocudu::ocuup
