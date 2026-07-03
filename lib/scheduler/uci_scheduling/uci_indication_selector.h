// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/circular_vector.h"
#include "ocudu/adt/stable_id_map.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"

namespace ocudu {

/// Action to be taken on the reception of a UCI indication PDU.
struct uci_action {
  enum class pdu_type : uint8_t { pucch_f0f1, pucch_f2f3f4, pusch };

  /// Which type of UCI indication event led to this action.
  pdu_type type = pdu_type::pucch_f0f1;
  /// Whether the decoding of the UCI was successful.
  bool uci_valid = false;
  /// Whether an SR was detected.
  bool sr_detected = false;
  /// HARQ-ACK bits.
  bounded_bitset<MAX_NOF_HARQS> harq_ack_bits;
  std::optional<float>          ul_sinr_dB;
  /// Timing Advance Offset measured for the UE.
  std::optional<phy_time_unit>   time_advance_offset;
  std::optional<csi_report_data> csi;
};

/// Notifier for UCI grants whose respective UCI indication feedback did not arrive to the scheduler before a timeout.
class uci_indication_timeout_notifier
{
public:
  virtual ~uci_indication_timeout_notifier() = default;

  /// Notifies that an UCI grant did not receive all the expected UCI PDUs before a deadline, and provides the course
  /// of action for the UCI grant.
  virtual void on_timeout(slot_point sl_rx, rnti_t crnti, const uci_action& action) = 0;
};

/// This class processes the scheduled PUCCH and PUSCH+UCI grants and the received UCI indication feedback from lower
/// layers and determines:
/// - if there are UCI grants that never received all the expected UCI feedback within a given timeout window.
/// - combines UCI indication PDUs into a single action for the case that a given UCI leads to more than one PUCCH
/// allocation (e.g. PUCCH F1 HARQ and HARQ-SR case).
class uci_indication_selector
{
public:
  /// \brief Timeout value to use when the PUCCH has been ACKed/NACKed, but it is expecting another PUCCH before being
  /// cleared (implementation-defined).
  static constexpr unsigned SHORT_PUCCH_TIMEOUT_SLOTS = 8U;
  /// \brief Default timeout in slots after which the HARQ process assumes that the CRC/ACK went missing
  /// (implementation-defined).
  static constexpr unsigned DEFAULT_ACK_TIMEOUT_SLOTS = 256U;

  uci_indication_selector(uci_indication_timeout_notifier& timeout_notifier,
                          unsigned                         ack_timeout_slots         = DEFAULT_ACK_TIMEOUT_SLOTS,
                          unsigned                         max_pucch_grants_per_slot = MAX_PUCCH_PDUS_PER_SLOT,
                          std::optional<float>             pucch_sinr_threshold_dB   = std::nullopt);

  std::optional<uci_action> handle_uci_ind_pdu(slot_point sl_rx, const uci_indication::uci_pdu& pdu);

  /// Called on every slot indication when a scheduler result is produced.
  void handle_result(slot_point sl_tx, const sched_result& result);

  /// \brief Called when an error indication is received for a given slot to reset all UCI grants with pending feedback.
  void handle_discarded_ucis(slot_point sl_tx);

private:
  static constexpr stable_id_t invalid_entry_id{std::numeric_limits<uint32_t>::max()};

  /// \brief Represents a scheduled UCI grant that is waiting for its respective UCI feedback.
  /// \remark An UCI entry can represent a PUSCH with UCI grant or one or more PUCCH grants (PUCCH F1 HARQ+SR case).
  struct uci_entry {
    rnti_t crnti = rnti_t::INVALID_RNTI;
    /// Number of UCI PDUs that need to be combined until a decision is made relative to the UCI outcome.
    uint8_t uci_pdus_to_rx = 0;
    /// Buffered action when several UCI PDUs need to be combined.
    uci_action chosen_action;
    /// Slot at which the UCI grant was scheduled (i.e. the slot used as key in the main UCI wheel).
    slot_point uci_slot;
    /// Next element in the linked list of UCI entries expected for a given slot Rx.
    stable_id_t next = invalid_entry_id;
    /// Next element in the linked list of UCI entries that will expire in a given slot if the remaining UCI PDUs
    /// for this grant do not arrive to the scheduler.
    stable_id_t next_short_timeout = invalid_entry_id;
    /// Slot at which the UCI entry will expire if the remaining UCI PDUs do not arrive on time.
    slot_point short_timeout_slot;
  };

  /// Handle UCI grant timeouts.
  void handle_timeouts(slot_point sl_tx);

  /// Handle a received UCI indication PDU and generate an action.
  std::optional<uci_action> handle_uci_pdu(const uci_indication::uci_pdu& pdu, uci_entry& entry);

  /// Convert a UCI indication PDU to an action.
  uci_action create_action(const uci_indication::uci_pdu& pdu) const;

  /// Called when a timeout occurs for a given pending UCI.
  void handle_timeout_pending_uci_entry(stable_id_t id, slot_point sl_rx);

  /// Called when the number of skipped slots is larger than the size of the UCI wheel.
  void handle_large_slot_jump(unsigned slot_jump);

  /// Timeout to receive HARQ-ACK feedback.
  const unsigned ack_timeout_slots;
  /// SINR threshold, in dB, below which PUCCH PDUs are considered invalid.
  /// If not set, no SINR-based filtering is applied.
  const std::optional<float>       pucch_sinr_threshold_dB;
  uci_indication_timeout_notifier& timeout_notifier;
  ocudulog::basic_logger&          logger;

  slot_point last_sl_tx;

  /// Shared pool of UCI entries.
  stable_id_map<uci_entry> uci_pool;

  /// \brief Each element of the circular vector maps a slot to a linked list with the UCI entries expected to be
  /// received in that slot.
  circular_vector<stable_id_intrusive_list<&uci_entry::next>> uci_wheel;
  /// \brief Each element of the circular vector maps a slot to a linked list with the UCI entries expected to timeout
  /// in that slot.
  circular_vector<stable_id_intrusive_list<&uci_entry::next_short_timeout>> short_timeout_wheel;
};

} // namespace ocudu
