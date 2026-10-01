// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
  /// \brief Slot of the UCI grant whose outcome this action carries.
  ///
  /// This is the slot the HARQ-ACK feedback timing points at, and therefore the one the respective DL HARQ processes
  /// are keyed on. In the case of a multi-slot PUCCH repetition burst, it is the slot of the first repetition, which
  /// can be earlier than the slot in which the UCI PDU was received.
  slot_point uci_slot;
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
/// - reduces the UCI indication PDUs of a multi-slot PUCCH repetition burst (TS 38.213, Section 9.2.6), one per
/// repetition slot, to a single action for the UCI grant that the burst carries.
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
                          unsigned                         max_nof_ue_contexts       = MAX_NOF_DU_UES_PER_CELL,
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
    /// \brief State of an entry that tracks a multi-slot PUCCH repetition burst as a whole (the \e anchor entry).
    ///
    /// All the repetitions of a burst carry the same UCI, so the burst is tracked by the entry of its first slot, whose
    /// UCI slot is therefore the first slot of the burst.
    struct burst_anchor {
      /// Whether the outcome of the burst was already forwarded to the scheduler.
      bool reported = false;
      /// Whether the last repetition of the burst was already scheduled.
      bool ended = false;
    };

    /// \brief State of an entry of one of the repetition slots that follow the first one of a burst.
    ///
    /// These entries hold no state of their own: they only point at the anchor entry, so that the UCI PDUs received in
    /// their slot reach it.
    struct burst_repetition {
      /// Slot of the first transmission of the burst, i.e. the UCI slot of its anchor entry.
      slot_point anchor_slot;
    };

    /// Role of this entry in the multi-slot PUCCH repetition burst carrying its UCI; empty if there is no such burst.
    std::variant<std::monostate, burst_anchor, burst_repetition> burst;

    /// Whether the UCI of this entry is carried by a multi-slot PUCCH repetition burst.
    [[nodiscard]] bool is_burst() const { return not std::holds_alternative<std::monostate>(burst); }

    /// Whether this entry tracks a multi-slot PUCCH repetition burst as a whole.
    [[nodiscard]] bool is_burst_anchor() const { return std::holds_alternative<burst_anchor>(burst); }

    /// Whether the outcome of the burst this entry belongs to was already forwarded to the scheduler.
    [[nodiscard]] bool is_burst_reported() const
    {
      const auto* anchor = std::get_if<burst_anchor>(&burst);
      return anchor != nullptr and anchor->reported;
    }

    /// \brief Slot of the first transmission of the burst carrying the UCI of this entry.
    /// \remark Only valid for entries that are part of a burst.
    [[nodiscard]] slot_point burst_anchor_slot() const
    {
      ocudu_assert(is_burst(), "Entry is not part of a PUCCH repetition burst");
      const auto* rep = std::get_if<burst_repetition>(&burst);
      return rep != nullptr ? rep->anchor_slot : uci_slot;
    }
  };

  /// Handle UCI grant timeouts.
  void handle_timeouts(slot_point sl_tx);

  /// Handle a received UCI indication PDU and generate an action.
  std::optional<uci_action> handle_uci_pdu(const uci_indication::uci_pdu& pdu, uci_entry& entry);

  /// Convert a UCI indication PDU to an action.
  uci_action create_action(const uci_indication::uci_pdu& pdu) const;

  /// Called when a timeout occurs for a given pending UCI.
  void handle_timeout_pending_uci_entry(stable_id_t id, slot_point sl_rx);

  /// Removes an UCI entry from the timeout wheels and from the entry pool.
  void erase_uci_entry(stable_id_t id);

  /// Registers a scheduled PUCCH grant that is one of the repetitions of a multi-slot PUCCH repetition burst.
  void handle_burst_pucch_grant(slot_point sl_tx, const pucch_info& pucch);

  /// \brief Retrieves the entry tracking the multi-slot PUCCH repetition burst of a UE that starts at a given slot.
  /// \return The id of the anchor entry of the burst; \c invalid_entry_id if the burst is not being tracked anymore.
  stable_id_t find_burst_anchor(rnti_t crnti, slot_point anchor_slot);

  /// Handle a received UCI indication PDU that belongs to a multi-slot PUCCH repetition burst.
  std::optional<uci_action> handle_burst_uci_pdu(slot_point anchor_slot, const uci_indication::uci_pdu& pdu);

  /// \brief Accounts for one repetition of a multi-slot PUCCH repetition burst, either received (\c action set) or
  /// lost (\c action equal to \c nullptr), and determines the resulting action for the burst, if any.
  ///
  /// As all the repetitions of a burst carry the same UCI, the first one that is successfully decoded already
  /// determines the outcome of the burst: it is forwarded right away, without waiting for the remaining repetitions,
  /// whose feedback is then silently absorbed by the anchor entry.
  std::optional<uci_action> handle_burst_repetition(stable_id_t anchor_id, const uci_action* action);

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
  circular_vector<stable_id_intrusive_list<&uci_entry::next>, true> uci_wheel;
  /// \brief Each element of the circular vector maps a slot to a linked list with the UCI entries expected to timeout
  /// in that slot.
  circular_vector<stable_id_intrusive_list<&uci_entry::next_short_timeout>, true> short_timeout_wheel;
};

} // namespace ocudu
