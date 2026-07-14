// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mac_scheduler_cell_info_handler.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/lockfree_triple_buffer.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/support/timers.h"

namespace ocudu {

/// Entity responsible for fetching encoded SIB1 and SI messages based on scheduled SI grants.
class sib_pdu_assembler
{
public:
  /// Encoder of BCCH-DL-SCH messages that require dynamic fields to be updated at each transmission (e.g., HyperSFN,
  /// segment cycling, or PWS content).
  class bcch_dl_sch_msg_encoder
  {
  public:
    virtual ~bcch_dl_sch_msg_encoder() = default;

    /// \brief Get an encoded BCCH-DL-SCH message buffer for a given slot point and SI scheduling occasion.
    /// \param[in] sl_tx   Transmission slot point.
    /// \param[in] si_info Scheduling occasion information (TBS, repetition/transmission count, etc.).
    /// \return The encoded BCCH-DL-SCH message buffer on success, otherwise an error containing the minimum TBS that
    /// should have been scheduled.
    virtual expected<span<const uint8_t>, units::bytes> encode(slot_point_extended    sl_tx,
                                                               const sib_information& si_info) = 0;
  };

  class message_handler
  {
  public:
    virtual ~message_handler() = default;

    virtual si_version_type update(si_version_type si_version, const byte_buffer& pdu) = 0;

    /// \brief Enqueue encodes SI messages at proper Tx slots.
    virtual bool enqueue_si_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req) = 0;

    /// Retrieve encoded SI bytes for a given SI scheduling opportunity.
    virtual span<const uint8_t> get_pdu(slot_point_extended sl_tx, const sib_information& si_info) = 0;
  };

  sib_pdu_assembler(du_cell_index_t                  cell_index,
                    const mac_cell_sys_info_config&  req,
                    timer_factory                    timers,
                    mac_scheduler_cell_info_handler& sched);
  ~sib_pdu_assembler();

  /// Update the SIB1 and SI messages.
  void handle_si_change_request(const mac_cell_sys_info_config& req);

  /// \brief Retrieve the encoded SI message.
  span<const uint8_t> encode_si_pdu(slot_point_extended sl_tx, const sib_information& si_info);

  /// \brief Handles an SI message PDU update. If \c pdu_update_req.pws_broadcast is set, this is routed to the PWS
  /// (Write-Replace Warning) broadcast content push and repetition sequence; otherwise it is a plain SI PDU update
  /// enqueued at its proper Tx slots.
  bool handle_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req);

private:
  using bcch_dl_sch_buffer = std::shared_ptr<const std::vector<uint8_t>>;

  /// A single BCCH-DL-SCH segment: its linearized (over-allocated) buffer, and its true byte length. Segments of the
  /// same SI-message are not guaranteed to have equal length (e.g. the last segment of a PWS warning message).
  struct bcch_segment {
    bcch_dl_sch_buffer buffer;
    units::bytes       len;
  };

  /// A snapshot of a SIB1 and SI messages within a given SI change window.
  struct si_buffer_snapshot {
    unsigned version;
    /// Encoder used to generate BCCH-DL-SCH SIB1 messages.
    std::shared_ptr<bcch_dl_sch_msg_encoder> sib1;
    /// Encoders used to generate each SI-message, indexed by SI-message index. An entry is null if the corresponding
    /// index does not exist.
    std::vector<std::shared_ptr<bcch_dl_sch_msg_encoder>> si_msg_encoders;
  };

  class sib1_assembler;

  /// \brief Encoder for a static (non-PWS) SI-message, replaced wholesale whenever its content changes.
  ///
  /// Static SI-messages are never segmented -- only PWS (SIB6/7/8) content segments across multiple SI-message
  /// windows (see \c pws_si_msg_encoder).
  class static_si_msg_encoder;

  /// \brief Encoder owning the repeat/count timing and content of an on-going PWS (Write-Replace Warning) broadcast
  /// sequence for a single SI-message index.
  ///
  /// Unlike \c static_si_msg_encoder, this object is created once (for SI-message indices that require activation,
  /// see \c si_message_scheduling_config::requires_activation) and persists across unrelated SI reconfigurations --
  /// it is mutated in place by \c handle_pws_broadcast rather than being replaced, since it owns a live repeat timer
  /// that must survive across CU-driven Write-Replace Warning content pushes.
  class pws_si_msg_encoder;

  void save_buffers(si_version_type si_version, const mac_cell_sys_info_config& req);

  /// \brief Enqueue encodes SI messages at proper Tx slots.
  bool enqueue_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req);

  /// \brief Handles a new PWS (Write-Replace Warning) broadcast content push and repetition sequence.
  bool handle_pws_broadcast(const mac_cell_sys_info_pdu_update& pdu_update_req);

  ocudulog::basic_logger& logger;

  du_cell_index_t                  cell_index;
  timer_factory                    timers;
  mac_scheduler_cell_info_handler& sched;

  // Last SI messages received by the assembler.
  static_vector<bcch_dl_sch_payload_type, MAX_SI_MESSAGES> last_si_messages;

  // SI buffers of last SI message update request.
  // Note: This member is only accessed from the control executor.
  si_buffer_snapshot last_cfg_buffers;

  // Buffers being transferred from configuration plane to assembler RT path.
  lockfree_triple_buffer<si_buffer_snapshot> pending;

  // SI buffers that are being currently encoded and sent to lower layers.
  // Note: This member is only accessed from the RT path.
  si_buffer_snapshot current_buffers;

  // Handler of SIB1s with extended functionality for eDRX.
  std::unique_ptr<sib1_assembler> sib1_hdlr;

  std::unique_ptr<message_handler> message_ext_handler;

  // PWS encoders, one entry per SI-message index. Only indices that require activation (see
  // si_message_scheduling_config::requires_activation) have a non-null entry -- this is decided once at
  // construction time, so checking for null is safe from the real-time path without further synchronization.
  // save_buffers() checks this to leave PWS-owned indices untouched during unrelated SI reconfigurations.
  std::vector<std::shared_ptr<pws_si_msg_encoder>> pws_encoders;
};

/// \brief Instantiates an SI message extension handler.
/// \param[in] req    Request containing System Information signalled by the cell.
/// \return A pointer to the SI message extension handler on success, otherwise \c nullptr.
std::unique_ptr<sib_pdu_assembler::message_handler>
create_si_message_extension_handler(const mac_cell_sys_info_config& req);

} // namespace ocudu
