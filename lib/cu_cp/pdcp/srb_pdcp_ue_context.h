// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/pdcp/pdcp_factory.h"
#include "ocudu/pdcp/pdcp_rx.h"
#include "ocudu/pdcp/pdcp_tx.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <array>
#include <functional>

namespace ocudu::ocucp {

// ---------------------------------------------------------------------------
// Internal helper types (not part of the public interface)
// ---------------------------------------------------------------------------

struct rrc_ue_rx_pdu_info {
  byte_buffer rrc_pdu;
  bool        integrity_verified = false;
};

struct pdcp_rx_result {
  std::variant<std::vector<rrc_ue_rx_pdu_info>, ngap_cause_t> result;

  bool         is_successful() const { return std::holds_alternative<std::vector<rrc_ue_rx_pdu_info>>(result); }
  ngap_cause_t get_failure_cause() const { return std::get<ngap_cause_t>(result); }
  std::vector<rrc_ue_rx_pdu_info> pop_pdus() { return std::move(std::get<std::vector<rrc_ue_rx_pdu_info>>(result)); }
};

/// Captures PDCP PDU emitted by PDCP TX (synchronous, inline executor).
class pdcp_srb_tx_adapter : public pdcp_tx_lower_notifier
{
public:
  void        on_new_pdu(byte_buffer pdu, bool /*is_retx*/) override { pdcp_pdu = std::move(pdu); }
  void        on_discard_pdu(uint32_t /*pdcp_sn*/) override {}
  byte_buffer get_pdcp_pdu() { return std::move(pdcp_pdu); }

private:
  byte_buffer pdcp_pdu;
};

/// Captures TX control events (failures) from PDCP TX.
class pdcp_srb_tx_control_adapter : public pdcp_tx_upper_control_notifier
{
public:
  void on_protocol_failure() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning(
        "Requesting UE release. Cause: Received protocol failure from PDCP Tx");
    cause = cause_protocol_t::unspecified;
  }
  void on_max_count_reached() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Max count reached from PDCP Tx");
    cause = cause_protocol_t::unspecified;
  }
  void on_resume_required() override
  {
    ocudulog::fetch_basic_logger("PDCP").error("Unsupported request for SRB resume from PDCP Tx");
  }
  ngap_cause_t get_failure_cause() { return cause; }

private:
  ngap_cause_t cause;
};

/// Collects decrypted RRC SDUs (and failures) from PDCP RX (synchronous, inline executor).
class pdcp_srb_rx_adapter : public pdcp_rx_upper_data_notifier, public pdcp_rx_upper_control_notifier
{
public:
  void on_new_sdu(byte_buffer sdu, bool integrity_verified) override
  {
    rrc_pdus.push_back(rrc_ue_rx_pdu_info{.rrc_pdu = std::move(sdu), .integrity_verified = integrity_verified});
  }
  void on_protocol_failure() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning(
        "Requesting UE release. Cause: Received protocol failure from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }
  void on_integrity_failure() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning(
        "Requesting UE release. Cause: Received integrity failure from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }
  void on_max_count_reached() override
  {
    ocudulog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Max count reached from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }
  void on_resume_required() override
  {
    ocudulog::fetch_basic_logger("PDCP").error("Unsupported request for SRB resume from PDCP Rx");
  }

  std::variant<std::vector<rrc_ue_rx_pdu_info>, ngap_cause_t> pop_result()
  {
    if (cause.has_value()) {
      auto ret = *cause;
      cause.reset();
      return ret;
    }
    return std::move(rrc_pdus);
  }

private:
  std::vector<rrc_ue_rx_pdu_info> rrc_pdus;
  std::optional<ngap_cause_t>     cause;
};

// ---------------------------------------------------------------------------
// Per-SRB PDCP entity bundle
// ---------------------------------------------------------------------------

struct srb_pdcp_entry {
  inline_task_executor         inline_executor;
  std::unique_ptr<pdcp_entity> entity;
  pdcp_srb_tx_adapter          pdcp_tx_notifier;
  pdcp_srb_tx_control_adapter  rrc_tx_control_notifier;
  pdcp_srb_rx_adapter          rrc_rx_data_notifier;
};

// ---------------------------------------------------------------------------
// srb_pdcp_ue_context — owns all SRB PDCP entities for one UE
// ---------------------------------------------------------------------------

/// Owns and manages PDCP entities for a single UE's SRBs (SRB1 and SRB2).
class srb_pdcp_ue_context : public rrc_ue_pdcp_notifier
{
public:
  srb_pdcp_ue_context(cu_cp_ue_index_t ue_index_,
                      timer_factory    timers_,
                      task_executor&   executor_,
                      uint32_t         max_nof_crypto_workers_) :
    ue_index(ue_index_), timers(timers_), executor(&executor_), max_nof_crypto_workers(max_nof_crypto_workers_)
  {
  }

  /// Connect the RRC UE as the consumer of decrypted UL SDUs and as the UE release trigger.
  void connect_rrc_ue(rrc_ul_pdu_handler& rrc_handler_, std::function<void(ngap_cause_t)> release_fn_)
  {
    rrc_handler = &rrc_handler_;
    release_fn  = std::move(release_fn_);
  }

  /// Called by f1ap_pdcp_ul_dcch_adapter to push an encrypted UL PDCP PDU into PDCP RX.
  void handle_ul_dcch_pdu(srb_id_t srb_id, byte_buffer pdcp_pdu)
  {
    unsigned idx = srb_id_to_uint(srb_id);
    if (idx >= MAX_NOF_SRBS || !srbs[idx].has_value()) {
      ocudulog::fetch_basic_logger("PDCP").error("Dropping UL-DCCH PDU. {} is not set up", srb_id);
      return;
    }
    srb_pdcp_entry& entry = *srbs[idx];

    auto chain = byte_buffer_chain::create(std::move(pdcp_pdu));
    if (!chain.has_value()) {
      trigger_release(ngap_cause_misc_t::not_enough_user_plane_processing_res);
      return;
    }

    entry.entity->get_rx_lower_interface().handle_pdu(std::move(chain.value()));

    auto rx_result = entry.rrc_rx_data_notifier.pop_result();
    if (std::holds_alternative<ngap_cause_t>(rx_result)) {
      trigger_release(std::get<ngap_cause_t>(rx_result));
      return;
    }

    auto& pdus = std::get<std::vector<rrc_ue_rx_pdu_info>>(rx_result);
    if (rrc_handler != nullptr) {
      for (auto& pdu_info : pdus) {
        rrc_handler->handle_ul_dcch_pdu(srb_id, std::move(pdu_info.rrc_pdu), pdu_info.integrity_verified);
      }
    }
  }

  // rrc_ue_pdcp_notifier

  bool has_srb(srb_id_t srb_id) const override
  {
    unsigned idx = srb_id_to_uint(srb_id);
    return idx < MAX_NOF_SRBS && srbs[idx].has_value();
  }

  void create_srb(srb_id_t srb_id) override
  {
    auto& entry = srbs[srb_id_to_uint(srb_id)].emplace();

    pdcp_entity_creation_message msg{};
    msg.ue_index               = cu_cp_ue_index_to_uint(ue_index);
    msg.rb_id                  = srb_id;
    msg.config                 = pdcp_make_default_srb_config();
    msg.tx_lower               = &entry.pdcp_tx_notifier;
    msg.tx_upper_cn            = &entry.rrc_tx_control_notifier;
    msg.rx_upper_dn            = &entry.rrc_rx_data_notifier;
    msg.rx_upper_cn            = &entry.rrc_rx_data_notifier;
    msg.ue_dl_timer_factory    = timers;
    msg.ue_ul_timer_factory    = timers;
    msg.ue_ctrl_timer_factory  = timers;
    msg.ue_dl_executor         = &entry.inline_executor;
    msg.ue_ul_executor         = &entry.inline_executor;
    msg.ue_ctrl_executor       = executor;
    msg.crypto_executor        = &entry.inline_executor;
    msg.max_nof_crypto_workers = max_nof_crypto_workers;

    entry.entity = create_pdcp_entity(msg);
  }

  pdcp_tx_result encrypt_pdu(srb_id_t srb_id, byte_buffer pdu) override
  {
    srb_pdcp_entry& entry = *srbs[srb_id_to_uint(srb_id)];
    entry.entity->get_tx_upper_data_interface().handle_sdu(std::move(pdu));

    byte_buffer packed = entry.pdcp_tx_notifier.get_pdcp_pdu();
    if (packed.empty()) {
      return pdcp_tx_result{entry.rrc_tx_control_notifier.get_failure_cause()};
    }
    return pdcp_tx_result{std::move(packed)};
  }

  void enable_tx_security(srb_id_t                    srb_id,
                          security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) override
  {
    srbs[srb_id_to_uint(srb_id)]->entity->get_tx_upper_control_interface().configure_security(
        sec_cfg, int_enabled, ciph_enabled);
  }

  void enable_rx_security(srb_id_t                    srb_id,
                          security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) override
  {
    srbs[srb_id_to_uint(srb_id)]->entity->get_rx_upper_control_interface().configure_security(
        sec_cfg, int_enabled, ciph_enabled);
  }

  void reestablish(srb_id_t srb_id, security::sec_128_as_config sec_cfg) override
  {
    srbs[srb_id_to_uint(srb_id)]->entity->get_tx_upper_control_interface().reestablish(sec_cfg);
    srbs[srb_id_to_uint(srb_id)]->entity->get_rx_upper_control_interface().reestablish(sec_cfg);
  }

  static_vector<srb_id_t, MAX_NOF_SRBS> get_srb_ids() const override
  {
    static_vector<srb_id_t, MAX_NOF_SRBS> ids;
    for (unsigned i = 0; i < MAX_NOF_SRBS; ++i) {
      if (srbs[i].has_value()) {
        ids.push_back(int_to_srb_id(i));
      }
    }
    return ids;
  }

private:
  void trigger_release(ngap_cause_t cause)
  {
    if (release_fn) {
      release_fn(cause);
    }
  }

  cu_cp_ue_index_t ue_index;
  timer_factory    timers;
  task_executor*   executor               = nullptr;
  uint32_t         max_nof_crypto_workers = 0;

  rrc_ul_pdu_handler*               rrc_handler = nullptr;
  std::function<void(ngap_cause_t)> release_fn;

  std::array<std::optional<srb_pdcp_entry>, MAX_NOF_SRBS> srbs = {};
};

} // namespace ocudu::ocucp
