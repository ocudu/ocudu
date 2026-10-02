// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_manager.h"

namespace ocudu::ocuup {

/// Adapter between PDCP and SDAP.
class pdcp_sdap_adapter : public pdcp_rx_upper_data_notifier
{
public:
  pdcp_sdap_adapter()           = default;
  ~pdcp_sdap_adapter() override = default;

  void connect_sdap(sdap_rx_pdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  // See interface for documentation.
  void on_new_sdu(byte_buffer sdu, bool integrity_verified) override
  {
    // The value of integrity_verified is unused for DRBs.
    ocudu_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    if (sdap_handler == nullptr) {
      ocudulog::fetch_basic_logger("GTPU", false).debug("Dropped PDCP SDU. Adapter is disconnected.");
      return;
    }

    sdap_handler->handle_pdu(std::move(sdu));
  }

private:
  sdap_rx_pdu_handler* sdap_handler = nullptr;
};

/// Adapter between PDCP Rx and E1AP (to be forwarded to RRC in the DU).
class pdcp_rx_cu_up_mngr_adapter : public pdcp_rx_upper_control_notifier
{
public:
  pdcp_rx_cu_up_mngr_adapter()           = default;
  ~pdcp_rx_cu_up_mngr_adapter() override = default;

  void connect_cu_up_mngr(cu_up_ue_index_t ue_index_, cu_up_manager_pdcp_interface* cu_up_mngr_)
  {
    cu_up_mngr = cu_up_mngr_;
    ue_index   = ue_index_;
  }

  // See interface for documentation.
  void on_protocol_failure() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").debug("Ignoring on_protocol_failure() from PDCP Rx: No E1AP handler.");
      return;
    }
    cu_up_mngr->handle_pdcp_protocol_failure(ue_index);
  }

  // See interface for documentation.
  void on_integrity_failure() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").debug("Ignoring on_integrity_failure() from PDCP Rx: No E1AP handler.");
      return;
    }
    cu_up_mngr->handle_pdcp_integrity_failure(ue_index);
  }

  // See interface for documentation.
  void on_max_count_reached() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").debug(
          "Max COUNT reached from PDCP Rx, but no E1AP handler present. Ignoring.");
      return;
    }
    cu_up_mngr->handle_pdcp_max_count_reached(ue_index);
  }

  // See interface for documentation.
  void on_resume_required() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").debug(
          "Resume requested from PDCP Rx, but no E1AP handler present. Ignoring");
      return;
    }
    cu_up_mngr->handle_pdcp_resume_required(ue_index);
  }

  cu_up_manager_pdcp_interface* cu_up_mngr = nullptr;
  cu_up_ue_index_t              ue_index   = INVALID_CU_UP_UE_INDEX;
};

/// Adapter between PDCP and F1-U
class pdcp_f1u_adapter : public pdcp_tx_lower_notifier
{
public:
  pdcp_f1u_adapter()           = default;
  ~pdcp_f1u_adapter() override = default;

  void connect_f1u(f1u_tx_sdu_handler& f1u_handler_) { f1u_handler = &f1u_handler_; }
  void disconnect_f1u() { f1u_handler = nullptr; }

  // See interface for documentation.
  void on_new_pdu(byte_buffer pdu, bool is_retx) override
  {
    if (f1u_handler == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").info("Dropped DL PDU. F1-U handler is not connected");
      return;
    }

    f1u_handler->handle_sdu(std::move(pdu), is_retx);
  }

  // See interface for documentation.
  void on_discard_pdu(uint32_t pdcp_sn) override
  {
    if (f1u_handler == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").info("Dropped discard command for SN {}. F1-U handler is not connected",
                                                pdcp_sn);
      return;
    }

    f1u_handler->discard_sdu(pdcp_sn);
  }

private:
  f1u_tx_sdu_handler* f1u_handler = nullptr;
};

/// Adapter between PDCP Tx and E1AP (to be forwarded to RRC in the DU)
class pdcp_tx_cu_up_mngr_adapter : public pdcp_tx_upper_control_notifier
{
public:
  pdcp_tx_cu_up_mngr_adapter()           = default;
  ~pdcp_tx_cu_up_mngr_adapter() override = default;

  void connect_cu_up_mngr(cu_up_ue_index_t ue_index_, cu_up_manager_pdcp_interface* cu_up_mngr_)
  {
    cu_up_mngr = cu_up_mngr_;
    ue_index   = ue_index_;
  }

  // See interface for documentation.
  void on_protocol_failure() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning("Ignoring on_protocol_failure() from PDCP Tx: No E1AP handler");
  }

  // See interface for documentation.
  void on_max_count_reached() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").warning(
          "No CU-UP manager handler for PDCP Tx control events. All events will be ignored");
      return;
    }
    cu_up_mngr->handle_pdcp_max_count_reached(ue_index);
  }

  // See interface for documentation.
  void on_resume_required() override
  {
    if (cu_up_mngr == nullptr) {
      ocudulog::fetch_basic_logger("PDCP").debug(
          "Resume requested from PDCP Tx, but no E1AP handler present. Ignoring");
      return;
    }
    cu_up_mngr->handle_pdcp_resume_required(ue_index);
  }

private:
  cu_up_manager_pdcp_interface* cu_up_mngr = nullptr;
  cu_up_ue_index_t              ue_index   = INVALID_CU_UP_UE_INDEX;
};

} // namespace ocudu::ocuup
