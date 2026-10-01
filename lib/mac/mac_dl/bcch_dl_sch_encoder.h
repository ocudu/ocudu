// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/support/units.h"
#include <memory>
#include <vector>

namespace ocudu {

/// Max BCCH-DL-SCH PDU size. This value is implementation-defined.
constexpr unsigned MAX_BCCH_DL_SCH_PDU_SIZE = 2048;

/// Linearized BCCH-DL-SCH buffer, as required by the lower layers.
using bcch_dl_sch_buffer = std::shared_ptr<const std::vector<uint8_t>>;

/// \brief Converts a byte buffer into a linearized, over-allocated BCCH-DL-SCH buffer.
///
/// The buffer is over-allocated to account for padding. Resizing it after this point is not allowed, as it would
/// invalidate the spans passed to the lower layers.
bcch_dl_sch_buffer make_linear_bcch_dl_sch_buffer(const byte_buffer& pdu);

/// A single BCCH-DL-SCH segment: its linearized buffer, and its true byte length. Segments of the same SI-message are
/// not guaranteed to have equal length (e.g. the last segment of a PWS warning message).
struct bcch_segment {
  bcch_dl_sch_buffer buffer;
  units::bytes       len;
};

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

/// Notifier of the end of the broadcast of the warnings carried by an ETWS/CMAS SI epoch.
class pws_broadcast_end_notifier
{
public:
  virtual ~pws_broadcast_end_notifier() = default;

  /// \brief Notifies that the cell went back to broadcasting the System Information of the normal operation.
  /// \param[in] ended_version Version of the last ETWS/CMAS SI epoch broadcast by the cell.
  virtual void on_pws_broadcast_end(si_version_type ended_version) = 0;
};

/// Handler of SI-message PDU updates that are applied at a given Tx slot, bypassing the SI modification window.
class si_message_extension_handler
{
public:
  virtual ~si_message_extension_handler() = default;

  /// Enqueue encoded SI messages at their proper Tx slots.
  virtual bool enqueue_si_pdu_updates(const mac_cell_sys_info_pdu_update& pdu_update_req) = 0;

  /// Retrieve encoded SI bytes for a given SI scheduling opportunity. Returns an empty span if none apply.
  virtual span<const uint8_t> get_pdu(slot_point_extended sl_tx, const sib_information& si_info) = 0;
};

/// \brief Instantiates an SI message extension handler.
/// \param[in] req Request containing System Information signalled by the cell.
/// \return A pointer to the SI message extension handler on success, otherwise \c nullptr.
std::unique_ptr<si_message_extension_handler> create_si_message_extension_handler(const mac_cell_sys_info_config& req);

/// \brief New SI epoch to apply in a MAC cell.
///
/// Carries the encoders to use for the new epoch and the scheduling parameters the MAC scheduler needs to allocate
/// their grants.
struct si_update_command {
  /// SI epoch counter that the scheduler stamps on the SI grants of this epoch.
  si_version_type version = 0;
  /// Encoder used to generate BCCH-DL-SCH SIB1 messages.
  std::shared_ptr<bcch_dl_sch_msg_encoder> sib1;
  /// Encoders used to generate each SI-message, indexed by SI-message index. An entry is null if the corresponding
  /// index does not exist.
  static_vector<std::shared_ptr<bcch_dl_sch_msg_encoder>, MAX_SI_MESSAGES> si_msgs;
  /// Scheduling parameters of SIB1 and of each SI-message.
  si_scheduling_config si_sched_cfg;
  /// SI messages that are broadcasting a warning. Only filled for an ETWS/CMAS epoch.
  static_vector<pws_broadcasting_si_message, MAX_PWS_SI_MESSAGES> active_pws_si_messages;
};

} // namespace ocudu
