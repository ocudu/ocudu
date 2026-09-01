// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_dl/bcch_dl_sch_encoder.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/timers.h"

namespace ocudu {

class mac_dl_cell_controller;

/// \brief Entity that manages the System Information broadcast by a MAC cell.
///
/// It manages the updating of the BCCH-DL-SCH payloads and turns System Information updates into commands to apply in
/// the MAC cell.
class si_message_controller
{
public:
  /// \remark Starts the broadcast of the System Information in the cell, so \c dl_cell must be able to receive SI
  /// epochs.
  si_message_controller(du_cell_index_t                 cell_index,
                        const mac_cell_sys_info_config& sys_info,
                        timer_factory                   timers,
                        task_executor&                  ctrl_exec,
                        mac_dl_cell_controller&         dl_cell);
  ~si_message_controller();

  /// Command generated from the last System Information update.
  const si_update_command& last_command() const { return last_cmd; }

  /// Outcome of the requested System Information updates.
  struct si_update_result {
    /// Whether the System Information changed, and a new SI epoch was applied in the cell.
    bool si_updated = false;
    /// Whether the SI message PDU updates were enqueued.
    bool si_pdus_enqueued = false;
  };

  /// Handles the System Information updates requested for the cell, applying the SI epochs they generate.
  si_update_result handle_si_change_request(const std::optional<mac_cell_sys_info_config>&     new_sys_info,
                                            const std::optional<mac_cell_sys_info_pdu_update>& new_si_pdu_info);

private:
  /// Encoder for a static BCCH-DL-SCH SIB1 payload.
  class sib1_static_encoder;

  /// Encoder for a BCCH-DL-SCH SIB1 payload whose HyperSFN is auto-updated, when eDRX is enabled.
  class sib1_hypersfn_encoder;

  /// \brief Encoder for a static (non-PWS) SI-message, replaced wholesale whenever its content changes.
  class static_si_msg_encoder;

  /// \brief Encoder of the content of a PWS (Write-Replace Warning) broadcast, cycling through its segments.
  ///
  /// Its content is immutable, so a new warning is broadcast by replacing the encoder, which also restarts the
  /// segment cycle.
  class pws_si_msg_encoder;

  /// Hop from the RT path, where the end of a warning broadcast is detected, to the cell control context.
  class pws_broadcast_end_adapter;

  /// \brief Repeat/count sequence of the PWS (Write-Replace Warning) broadcasts of one SI message.
  ///
  /// Unlike the encoders, it persists across unrelated SI reconfigurations, since it owns a live repeat timer that
  /// must survive across CU-driven Write-Replace Warning content pushes.
  class pws_broadcast_sequence;

  /// Handles the end of the broadcast of the warnings that an ETWS/CMAS SI epoch carried.
  void handle_pws_broadcast_end(si_version_type ended_version);

  /// \brief Generates a new SI epoch and applies it in the cell.
  /// \return Whether the System Information changed, and hence an epoch was generated.
  bool push_si_epoch(const mac_cell_sys_info_config& req);

  /// \brief Handles an SI message PDU update. If \c req.pws_broadcast is set, this is routed to the PWS
  /// (Write-Replace Warning) broadcast content push and repetition sequence; otherwise it is a plain SI PDU update
  /// enqueued at its proper Tx slots.
  bool handle_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& req);

  /// Whether the System Information differs from the one the current encoders were built from.
  bool has_si_changed(const mac_cell_sys_info_config& req) const;

  /// Rebuilds the encoders that changed and updates the command to apply.
  void build_command(const mac_cell_sys_info_config& req);

  /// \brief Builds the SIB1 payload that the SI epoch broadcasting a given set of warnings carries.
  /// \param cell_sib1 SIB1 payload the DU packed for the cell.
  /// \param hypersfn_enabled Whether the SIB1 carries a hyper SFN that is patched at broadcast time.
  /// \param on_air SI messages carrying a warning. Empty for the epoch of the normal operation.
  /// \return The payload, or std::nullopt if the SIB1 of the cell could not be processed.
  std::optional<byte_buffer>
  make_epoch_sib1(const byte_buffer& cell_sib1, bool hypersfn_enabled, span<const sib_type_set> on_air) const;

  /// Creates the encoder of a SIB1 payload.
  std::shared_ptr<bcch_dl_sch_msg_encoder> make_sib1_encoder(const byte_buffer& sib1) const;

  /// Fills the scheduling parameters and SI-message encoders of the SI epoch broadcasting a given set of warnings.
  void fill_epoch_si_config(si_update_command& cmd, span<const sib_type_set> on_air, units::bytes sib1_len) const;

  /// \brief Fetches the encoder of the SI message carrying a given set of SIBs.
  /// \return The encoder, or nullptr if the cell holds no encoder for it.
  std::shared_ptr<bcch_dl_sch_msg_encoder> find_si_msg_encoder(sib_type_set sibs) const;

  /// Returns the SIBs carried by each SI message that is currently broadcasting a warning.
  static_vector<sib_type_set, MAX_PWS_SI_MESSAGES> on_air_sib_sets() const;

  bool handle_pws_broadcast(const mac_cell_sys_info_pdu_update& req);

  /// \brief Derives the ETWS/CMAS SI epoch from the current one and applies it in the cell.
  /// \param pws_sib_set SIB set of the SI message starting one more broadcast of its warning.
  void push_pws_epoch(std::optional<sib_type_set> pws_sib_set);

  /// \brief Fetches the PWS broadcast sequence of the SI message carrying a given set of SIBs.
  /// \return The sequence, or nullptr if no SI message carries them.
  pws_broadcast_sequence* find_pws_sequence(sib_type_set sib_set) const;

  ocudulog::basic_logger& logger;
  du_cell_index_t         cell_index;
  timer_factory           timers;
  task_executor&          ctrl_exec;
  mac_dl_cell_controller& dl_cell;

  // Last SIB1 payload received from the DU, from which every SI epoch is derived.
  byte_buffer last_sib1;
  bool        last_hypersfn_enabled = false;

  // Last SI messages used to build the current SI-message encoders.
  static_vector<bcch_dl_sch_payload_type, MAX_SI_MESSAGES> last_si_messages;

  // SI scheduling configuration of the cell, listing every SI message the cell can broadcast.
  si_scheduling_config cell_si_sched_cfg;

  // Encoders of the SI messages the cell can broadcast, indexed as in the cell SI scheduling configuration.
  static_vector<std::shared_ptr<bcch_dl_sch_msg_encoder>, MAX_SI_MESSAGES> cell_si_msgs;

  // Last SI epoch handed out. Both the normal operation and the ETWS/CMAS epochs draw from it, so that a version
  // identifies an epoch on its own.
  si_version_type last_version = 0;

  // Command matching the System Information currently being broadcast.
  si_update_command last_cmd;

  // SI messages that are currently carrying a warning.
  static_vector<pws_broadcasting_si_message, MAX_PWS_SI_MESSAGES> active_pws_si_msgs;

  std::shared_ptr<si_message_extension_handler> ext_handler;

  // PWS broadcast sequences, one entry per SI message carrying PWS SIBs, keyed by the SI message identity. Keying by
  // identity rather than by position keeps an on-going warning attached to its SIBs when the SI scheduling layout
  // changes.
  std::vector<std::pair<sib_type_set, std::unique_ptr<pws_broadcast_sequence>>> pws_sequences;
};

} // namespace ocudu
