// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// State tracking the UE's RRC configuration progress in the scheduler.
enum class ue_config_state : uint8_t {
  /// Awaiting confirmation that the UE applied the RRC Setup/Reestablishment/Resume.
  pending_initial_conf,
  /// Awaiting confirmation that the UE applied an RRC Reconfiguration.
  pending_reconf,
  /// Awaiting confirmation that the UE applied an RRC Reconfiguration following RRC Reestablishment.
  pending_reest_reconf,
  /// The UE configuration has been applied.
  config_applied,
};

/// State tracking the UE's contention resolution progress in the scheduler.
enum class ue_conres_state : uint8_t {
  /// RACH-created UE: waiting for the ConRes MAC CE to be ACKed by the UE.
  pending_conres_ce,
  /// F1AP-created UE: waiting for the C-RNTI MAC CE to be received to complete contention resolution.
  pending_conres_crnti_ce,
  /// Contention resolution is complete.
  conres_completed,
};

/// Whether the UE is in fallback mode (true when either state has not reached its terminal value).
inline bool is_in_fallback(ue_config_state cfg_st, ue_conres_state conres_st)
{
  return cfg_st != ue_config_state::config_applied or conres_st != ue_conres_state::conres_completed;
}

inline const char* to_string(ue_config_state state)
{
  switch (state) {
    case ue_config_state::pending_initial_conf:
      return "pending_initial_conf";
    case ue_config_state::pending_reconf:
      return "pending_reconf";
    case ue_config_state::pending_reest_reconf:
      return "pending_reest_reconf";
    case ue_config_state::config_applied:
      return "config_applied";
  }
  return "unknown";
}

inline const char* to_string(ue_conres_state state)
{
  switch (state) {
    case ue_conres_state::pending_conres_ce:
      return "pending_conres_ce";
    case ue_conres_state::pending_conres_crnti_ce:
      return "pending_crnti_ce";
    case ue_conres_state::conres_completed:
      return "conres_completed";
  }
  return "unknown";
}

/// Type of events that trigger UE FSM updates.
enum class ue_fsm_config_event : uint8_t {
  /// Contention Resolution MAC CE has been ACKed by the UE.
  conres_ce_acked,
  /// Timed out waiting for the Contention Resolution MAC CE to be ACKed (timer has expired).
  conres_ce_timeout,
  /// C-RNTI MAC CE has been received from the UE (F1AP-created UE path).
  crnti_ce_received,
  /// An RRC Reconfiguration following an RRC Reestablishment has been initiated.
  reest_reconf_initiated,
  /// A UE Context Setup Request from the CU has been received (may trigger a reconfiguration or be a no-op).
  ue_ctx_setup_received,
  /// The UE has confirmed that the pending RRC configuration has been applied.
  config_applied
};

inline const char* to_string(ue_fsm_config_event ev)
{
  switch (ev) {
    case ue_fsm_config_event::conres_ce_acked:
      return "conres_ce_acked";
    case ue_fsm_config_event::conres_ce_timeout:
      return "conres_ce_timeout";
    case ue_fsm_config_event::crnti_ce_received:
      return "crnti_ce_received";
    case ue_fsm_config_event::reest_reconf_initiated:
      return "reest_reconf_initiated";
    case ue_fsm_config_event::ue_ctx_setup_received:
      return "ue_context_setup_received";
    case ue_fsm_config_event::config_applied:
      return "config_applied";
  }
  return "unknown";
}

} // namespace ocudu
