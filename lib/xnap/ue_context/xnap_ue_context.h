// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "xnap_ue_logger.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/support/enum_utils.h"
#include "ocudu/support/timers.h"
#include "ocudu/xnap/xnap_types.h"
#include <algorithm>
#include <map>
#include <optional>

namespace ocudu::ocucp {

struct xnap_ue_ids {
  cu_cp_ue_index_t         ue_index         = cu_cp_ue_index_t::invalid;
  const local_xnap_ue_id_t local_xnap_ue_id = local_xnap_ue_id_t::invalid;
  peer_xnap_ue_id_t        peer_xnap_ue_id  = peer_xnap_ue_id_t::invalid;
};

/// Event source for the outcome of a Handover Preparation transaction (HANDOVER REQUEST ACKNOWLEDGE or HANDOVER
/// PREPARATION FAILURE).
using ho_prep_outcome_t = protocol_transaction_event_source<asn1::xnap::ho_request_ack_s, asn1::xnap::ho_prep_fail_s>;

struct xnap_ue_context {
  xnap_ue_ids ue_ids;
  // Keep the logger declared before the event sources. Destroying an event source cancels its pending transaction,
  // which resumes the awaiting procedure from within the destructor, and that procedure logs the cancellation.
  xnap_ue_logger logger;

  /// XN Handover Request Ack/Handover Preparation Failure Event Source, used for immediate (non-conditional) HO.
  ho_prep_outcome_t xn_handover_outcome;

  /// XN Status Transfer Event Source.
  protocol_transaction_event_source<asn1::xnap::sn_status_transfer_s> sn_status_transfer_outcome;

  /// Retrieve UE Context Response/Failure Event Source.
  protocol_transaction_event_source<asn1::xnap::retrieve_ue_context_resp_s, asn1::xnap::retrieve_ue_context_fail_s>
      retrieve_ue_context_outcome;

  /// \brief Event sources of the in-flight CHO preparations, indexed by target cell.
  ///
  /// TS 38.423 Section 8.2.1.1 allows parallel Handover Preparation transactions for a conditional handover; they
  /// share one Source NG-RAN node UE XnAP ID and are told apart by the target cell. std::map is node-based, so a
  /// pointer handed to a running procedure stays valid while other cells are added or removed.
  std::map<nr_cell_global_id_t, ho_prep_outcome_t> cho_cell_outcomes;

  /// \brief CHO candidates prepared at this peer: target cell -> Target NG-RAN node UE XnAP ID.
  std::map<nr_cell_global_id_t, peer_xnap_ue_id_t> cho_prepared;

  /// \brief Cell this context was prepared for while acting as the handover target, if any.
  ///
  /// Parallel CHO preparations from one source share the Source NG-RAN node UE XnAP ID, which therefore does not
  /// identify a single context here. Messages that omit the Target NG-RAN node UE XnAP ID but name a candidate cell
  /// (e.g. a HANDOVER CANCEL, TS 38.423 Section 8.2.3.2) are resolved with this.
  std::optional<nr_cell_global_id_t> ho_target_cell;

  xnap_ue_context(cu_cp_ue_index_t        ue_index_,
                  local_xnap_ue_id_t      local_xnap_ue_id_,
                  timer_factory           timer_db,
                  ocudulog::basic_logger& logger_) :
    ue_ids({ue_index_, local_xnap_ue_id_}),
    logger("XNAP", {ue_index_, local_xnap_ue_id_}),
    xn_handover_outcome(timer_db),
    sn_status_transfer_outcome(timer_db),
    retrieve_ue_context_outcome(timer_db)
  {
  }

  /// \brief Cancels all transactions pending for this UE, e.g. as part of XNAP shutdown.
  void cancel_all_transactions()
  {
    xn_handover_outcome.stop();
    sn_status_transfer_outcome.stop();
    retrieve_ue_context_outcome.stop();
    // Note: stopping an outcome resumes the awaiting procedure from within this loop. Procedures must not touch the
    // UE context on the cancellation path, or they would mutate the map being iterated here.
    for (auto& [cell, outcome] : cho_cell_outcomes) {
      outcome.stop();
    }
  }

  /// \brief Registers a new in-flight CHO preparation for the given target cell.
  /// \return Pointer to the event source the preparation procedure must subscribe to, or nullptr if a preparation is
  /// already in flight for this cell.
  ho_prep_outcome_t* add_cho_cell_prep(const nr_cell_global_id_t& cell, timer_factory timer_db)
  {
    auto [it, inserted] = cho_cell_outcomes.try_emplace(cell, timer_db);
    if (!inserted) {
      return nullptr;
    }
    return &it->second;
  }

  /// \brief Finds the event source of an in-flight CHO preparation, or nullptr if none is in flight for this cell.
  ho_prep_outcome_t* find_cho_cell_outcome(const nr_cell_global_id_t& cell)
  {
    auto it = cho_cell_outcomes.find(cell);
    return it != cho_cell_outcomes.end() ? &it->second : nullptr;
  }

  /// \brief Drops the event source of an in-flight CHO preparation that did not succeed.
  void remove_cho_cell_prep(const nr_cell_global_id_t& cell) { cho_cell_outcomes.erase(cell); }

  /// \brief Delivers a Handover Preparation outcome to the transaction that requested it.
  ///
  /// TS 38.423 Section 8.2.1.1: parallel CHO preparations share a Source NG-RAN node UE XnAP ID and are told apart
  /// by the target cell, which Section 8.2.1.2 requires the acknowledgement to echo back. The HANDOVER PREPARATION
  /// FAILURE may omit it (Section 9.1.1.3), so without a cell the outcome goes to an awaiting immediate handover,
  /// or else to the only CHO preparation in flight when there is exactly one.
  /// \return True if the outcome was delivered, false if no transaction could be identified.
  template <typename Outcome>
  bool deliver_ho_prep_outcome(const std::optional<nr_cell_global_id_t>& cell, const Outcome& outcome)
  {
    if (cell.has_value()) {
      ho_prep_outcome_t* cell_outcome = find_cho_cell_outcome(*cell);
      return cell_outcome != nullptr and cell_outcome->set(outcome);
    }
    // No cell named. TS 38.423 Section 8.2.1.3 makes the IE optional in a HandoverPreparationFailure, so this is a
    // conformant peer, but with several CHO preparations in flight nothing identifies which one it answers. Try the
    // immediate handover only while no CHO preparation is outstanding, so that a CHO answer cannot resolve one.
    if (cho_cell_outcomes.empty()) {
      return xn_handover_outcome.set(outcome);
    }
    if (cho_cell_outcomes.size() == 1) {
      return cho_cell_outcomes.begin()->second.set(outcome);
    }
    logger.log_warning("Discarding a handover preparation outcome that names no target cell: {} CHO preparations are "
                       "in flight and none can be singled out",
                       cho_cell_outcomes.size());
    return false;
  }

  /// \brief Returns the Target NG-RAN node UE XnAP ID prepared for a CHO candidate cell, or invalid if not prepared.
  peer_xnap_ue_id_t find_cho_prepared(const nr_cell_global_id_t& cell) const
  {
    auto it = cho_prepared.find(cell);
    return it != cho_prepared.end() ? it->second : peer_xnap_ue_id_t::invalid;
  }

  /// \brief Checks whether any prepared CHO candidate still holds the given Target NG-RAN node UE XnAP ID.
  bool holds_cho_peer_xnap_ue_id(peer_xnap_ue_id_t peer_xnap_ue_id) const
  {
    return std::any_of(cho_prepared.begin(), cho_prepared.end(), [peer_xnap_ue_id](const auto& prepared) {
      return prepared.second == peer_xnap_ue_id;
    });
  }

  /// \brief Promotes a successful CHO preparation to a prepared candidate.
  /// \brief Records a prepared CHO candidate, and returns the Target NG-RAN node UE XnAP ID it replaces, if any.
  ///
  /// TS 38.423 Section 8.2.1.2 lets a candidate cell be prepared again; the target then answers with a new Target
  /// NG-RAN node UE XnAP ID, and the previous one is no longer addressable.
  peer_xnap_ue_id_t mark_cho_prepared(const nr_cell_global_id_t& cell, peer_xnap_ue_id_t peer_xnap_ue_id)
  {
    cho_cell_outcomes.erase(cell);
    peer_xnap_ue_id_t replaced = peer_xnap_ue_id_t::invalid;
    auto              it       = cho_prepared.find(cell);
    if (it != cho_prepared.end() and it->second != peer_xnap_ue_id) {
      replaced = it->second;
    }
    cho_prepared.insert_or_assign(cell, peer_xnap_ue_id);
    return replaced;
  }

  /// \brief Drops a prepared CHO candidate.
  /// \return The Target NG-RAN node UE XnAP ID it held, or invalid if the cell was not prepared.
  peer_xnap_ue_id_t remove_cho_prepared(const nr_cell_global_id_t& cell)
  {
    auto it = cho_prepared.find(cell);
    if (it == cho_prepared.end()) {
      return peer_xnap_ue_id_t::invalid;
    }
    peer_xnap_ue_id_t peer_xnap_ue_id = it->second;
    cho_prepared.erase(it);
    return peer_xnap_ue_id;
  }

  /// \brief Checks whether this UE has no CHO preparation in flight and no prepared CHO candidate left.
  bool cho_idle() const { return cho_cell_outcomes.empty() and cho_prepared.empty(); }
};

class xnap_ue_context_list
{
public:
  xnap_ue_context_list(timer_manager& timers_, task_executor& ctrl_exec_, ocudulog::basic_logger& logger_) :
    timers(timers_), ctrl_exec(ctrl_exec_), logger(logger_)
  {
  }

  /// \brief Checks whether a UE with the given LOCAL XNAP UE ID exists.
  /// \param[in] xnap_ue_id The LOCAL XNAP UE ID used to find the UE.
  /// \return True when a UE for the given LOCAL XNAP UE ID exists, false otherwise.
  bool contains(local_xnap_ue_id_t xnap_ue_id) const { return ues.find(xnap_ue_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return True when a UE for the given UE index exists, false otherwise.
  bool contains(cu_cp_ue_index_t ue_index) const
  {
    if (ue_index_to_local_xnap_ue_id.find(ue_index) == ue_index_to_local_xnap_ue_id.end()) {
      return false;
    }
    if (ues.find(ue_index_to_local_xnap_ue_id.at(ue_index)) == ues.end()) {
      return false;
    }
    return true;
  }

  /// \brief Checks whether a UE with the given PEER XNAP UE ID exists.
  /// \param[in] peer_xnap_ue_id The PEER XNAP UE ID used to find the UE.
  /// \return True when a UE for the given PEER XNAP UE ID exists, false otherwise.
  bool contains(peer_xnap_ue_id_t peer_xnap_ue_id) const
  {
    if (peer_xnap_ue_id_to_local_xnap_ue_id.find(peer_xnap_ue_id) == peer_xnap_ue_id_to_local_xnap_ue_id.end()) {
      return false;
    }
    if (ues.find(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id)) == ues.end()) {
      return false;
    }
    return true;
  }

  xnap_ue_context& operator[](local_xnap_ue_id_t local_xnap_ue_id)
  {
    ocudu_assert(ues.find(local_xnap_ue_id) != ues.end(),
                 "local_xnap_ue={}: XNAP UE context not found",
                 fmt::underlying(local_xnap_ue_id));
    return ues.at(local_xnap_ue_id);
  }

  xnap_ue_context& operator[](cu_cp_ue_index_t ue_index)
  {
    ocudu_assert(ue_index_to_local_xnap_ue_id.find(ue_index) != ue_index_to_local_xnap_ue_id.end(),
                 "ue={}: XNAP UE ID not found",
                 ue_index);
    ocudu_assert(ues.find(ue_index_to_local_xnap_ue_id.at(ue_index)) != ues.end(),
                 "local_xnap_ue={}: XNAP UE context not found",
                 fmt::underlying(ue_index_to_local_xnap_ue_id.at(ue_index)));
    return ues.at(ue_index_to_local_xnap_ue_id.at(ue_index));
  }

  xnap_ue_context& operator[](peer_xnap_ue_id_t peer_xnap_ue_id)
  {
    ocudu_assert(peer_xnap_ue_id_to_local_xnap_ue_id.find(peer_xnap_ue_id) != peer_xnap_ue_id_to_local_xnap_ue_id.end(),
                 "peer_xnap_ue={}: local XNAP UE ID not found",
                 fmt::underlying(peer_xnap_ue_id));
    ocudu_assert(ues.find(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id)) != ues.end(),
                 "peer_xnap_ue={}: XNAP UE context not found",
                 fmt::underlying(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id)));
    return ues.at(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id));
  }

  xnap_ue_context* find(local_xnap_ue_id_t xnap_ue_id)
  {
    auto it = ues.find(xnap_ue_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
  }

  const xnap_ue_context* find(local_xnap_ue_id_t xnap_ue_id) const
  {
    auto it = ues.find(xnap_ue_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
  }

  xnap_ue_context* find(peer_xnap_ue_id_t peer_xnap_ue_id)
  {
    if (peer_xnap_ue_id_to_local_xnap_ue_id.find(peer_xnap_ue_id) == peer_xnap_ue_id_to_local_xnap_ue_id.end()) {
      return nullptr;
    }
    return find(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id));
  }

  const xnap_ue_context* find(peer_xnap_ue_id_t peer_xnap_ue_id) const
  {
    if (peer_xnap_ue_id_to_local_xnap_ue_id.find(peer_xnap_ue_id) == peer_xnap_ue_id_to_local_xnap_ue_id.end()) {
      return nullptr;
    }
    return find(peer_xnap_ue_id_to_local_xnap_ue_id.at(peer_xnap_ue_id));
  }

  xnap_ue_context* find(cu_cp_ue_index_t ue_index)
  {
    if (ue_index_to_local_xnap_ue_id.find(ue_index) == ue_index_to_local_xnap_ue_id.end()) {
      return nullptr;
    }
    return find(ue_index_to_local_xnap_ue_id.at(ue_index));
  }

  const xnap_ue_context* find(cu_cp_ue_index_t ue_index) const
  {
    if (ue_index_to_local_xnap_ue_id.find(ue_index) == ue_index_to_local_xnap_ue_id.end()) {
      return nullptr;
    }
    return find(ue_index_to_local_xnap_ue_id.at(ue_index));
  }

  xnap_ue_context& add_ue(cu_cp_ue_index_t ue_index, local_xnap_ue_id_t xnap_ue_id)
  {
    ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", fmt::underlying(ue_index));
    ocudu_assert(xnap_ue_id != local_xnap_ue_id_t::invalid, "Invalid xnap_ue_id={}", fmt::underlying(xnap_ue_id));

    logger.debug("ue={} xnap_ue={}: XNAP UE context created", fmt::underlying(ue_index), fmt::underlying(xnap_ue_id));
    ues.emplace(std::piecewise_construct,
                std::forward_as_tuple(xnap_ue_id),
                std::forward_as_tuple(ue_index, xnap_ue_id, timer_factory{timers, ctrl_exec}, logger));
    ue_index_to_local_xnap_ue_id.emplace(ue_index, xnap_ue_id);
    return ues.at(xnap_ue_id);
  }

  /// \brief Returns the LOCAL XNAP UE ID this UE uses towards this peer, allocating a UE context if needed.
  ///
  /// All CHO candidate cells served by this peer share one LOCAL XNAP UE ID (TS 38.423 Section 8.2.1.1).
  /// \return The LOCAL XNAP UE ID, or invalid when no ID could be allocated.
  local_xnap_ue_id_t find_or_create_ue_context(cu_cp_ue_index_t ue_index)
  {
    auto it = ue_index_to_local_xnap_ue_id.find(ue_index);
    if (it != ue_index_to_local_xnap_ue_id.end()) {
      return it->second;
    }
    local_xnap_ue_id_t local_xnap_ue_id = allocate_local_xnap_ue_id();
    if (local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
      return local_xnap_ue_id_t::invalid;
    }
    add_ue(ue_index, local_xnap_ue_id);
    return local_xnap_ue_id;
  }

  /// \brief Promotes a successful CHO preparation to a prepared candidate and registers its PEER XNAP UE ID.
  ///
  /// Looks the UE context up by ID rather than taking a reference, so that a preparation procedure can call this
  /// after having been suspended, when its UE context may already be gone.
  void mark_cho_prepared(local_xnap_ue_id_t         local_xnap_ue_id,
                         const nr_cell_global_id_t& cell,
                         peer_xnap_ue_id_t          peer_xnap_ue_id)
  {
    auto it = ues.find(local_xnap_ue_id);
    if (it == ues.end()) {
      return;
    }
    const peer_xnap_ue_id_t replaced = it->second.mark_cho_prepared(cell, peer_xnap_ue_id);
    // A re-prepared candidate leaves its previous ID unreachable; drop the lookup entry so a later reallocation of
    // that ID cannot resolve to this context.
    if (replaced != peer_xnap_ue_id_t::invalid) {
      erase_peer_xnap_ue_id_lookup(replaced, local_xnap_ue_id);
    }
    // Note: peer IDs are only unique per peer node, so a candidate may collide with one of another Xn interface.
    // Within this list all candidates share one peer, hence one ID space.
    peer_xnap_ue_id_to_local_xnap_ue_id.insert_or_assign(peer_xnap_ue_id, local_xnap_ue_id);
  }

  /// \brief Drops the event source of a CHO preparation that did not succeed.
  void remove_cho_cell_prep(local_xnap_ue_id_t local_xnap_ue_id, const nr_cell_global_id_t& cell)
  {
    auto it = ues.find(local_xnap_ue_id);
    if (it == ues.end()) {
      return;
    }
    it->second.remove_cho_cell_prep(cell);
  }

  /// \brief Drops a prepared CHO candidate and releases the PEER XNAP UE ID lookup it held, if nothing else uses it.
  ///
  /// The PEER XNAP UE ID is taken from the candidate itself rather than from the peer's message, so that a stale or
  /// misaddressed identifier can never unregister a lookup entry belonging to an unrelated UE.
  /// \return The Target NG-RAN node UE XnAP ID the candidate held, or invalid if the cell was not prepared.
  peer_xnap_ue_id_t remove_cho_prepared(local_xnap_ue_id_t local_xnap_ue_id, const nr_cell_global_id_t& cell)
  {
    auto it = ues.find(local_xnap_ue_id);
    if (it == ues.end()) {
      return peer_xnap_ue_id_t::invalid;
    }
    const peer_xnap_ue_id_t peer_xnap_ue_id = it->second.remove_cho_prepared(cell);
    if (peer_xnap_ue_id == peer_xnap_ue_id_t::invalid) {
      return peer_xnap_ue_id_t::invalid;
    }
    // Keep the reverse lookup while the UE-associated signalling connection, or another prepared candidate, still
    // uses this ID.
    if (it->second.ue_ids.peer_xnap_ue_id != peer_xnap_ue_id and
        !it->second.holds_cho_peer_xnap_ue_id(peer_xnap_ue_id)) {
      erase_peer_xnap_ue_id_lookup(peer_xnap_ue_id, local_xnap_ue_id);
    }
    return peer_xnap_ue_id;
  }

  /// \brief Finds the UE context this node prepared for \c cell towards the peer identified by \c peer_xnap_ue_id.
  ///
  /// Parallel CHO preparations from one source share the Source NG-RAN node UE XnAP ID, so it alone does not identify
  /// a context; the candidate cell tells them apart (TS 38.423 Section 8.2.1.1).
  xnap_ue_context* find_by_peer_and_cell(peer_xnap_ue_id_t peer_xnap_ue_id, const nr_cell_global_id_t& cell)
  {
    for (auto& ue : ues) {
      if (ue.second.ue_ids.peer_xnap_ue_id == peer_xnap_ue_id and ue.second.ho_target_cell.has_value() and
          *ue.second.ho_target_cell == cell) {
        return &ue.second;
      }
    }
    return nullptr;
  }

  void update_peer_xnap_ue_id(local_xnap_ue_id_t local_xnap_ue_id, peer_xnap_ue_id_t peer_xnap_ue_id)
  {
    ocudu_assert(
        peer_xnap_ue_id != peer_xnap_ue_id_t::invalid, "Invalid peer_xnap_ue_id={}", fmt::underlying(peer_xnap_ue_id));
    ocudu_assert(local_xnap_ue_id != local_xnap_ue_id_t::invalid,
                 "Invalid local_xnap_ue_id={}",
                 fmt::underlying(local_xnap_ue_id));
    ocudu_assert(ues.find(local_xnap_ue_id) != ues.end(),
                 "local_xnap_ue={}: XNAP UE context not found",
                 fmt::underlying(local_xnap_ue_id));

    auto& ue = ues.at(local_xnap_ue_id);

    if (ue.ue_ids.peer_xnap_ue_id == peer_xnap_ue_id) {
      // If the peer XNAP UE ID is already set, we don't want to change it.
      return;
    }

    if (ue.ue_ids.peer_xnap_ue_id == peer_xnap_ue_id_t::invalid) {
      // If it was not set before, we add it.
      ue.logger.log_debug("Setting peer_xnap_ue_id={}", fmt::underlying(peer_xnap_ue_id));
      ue.ue_ids.peer_xnap_ue_id = peer_xnap_ue_id;
      peer_xnap_ue_id_to_local_xnap_ue_id.emplace(peer_xnap_ue_id, local_xnap_ue_id);
    } else if (ue.ue_ids.peer_xnap_ue_id != peer_xnap_ue_id) {
      // If it was set before, we update it.
      peer_xnap_ue_id_t old_peer_xnap_ue_id = ue.ue_ids.peer_xnap_ue_id;
      ue.logger.log_info("Updating peer_xnap_ue_id={}", fmt::underlying(peer_xnap_ue_id));
      ue.ue_ids.peer_xnap_ue_id = peer_xnap_ue_id;
      peer_xnap_ue_id_to_local_xnap_ue_id.emplace(peer_xnap_ue_id, local_xnap_ue_id);
      peer_xnap_ue_id_to_local_xnap_ue_id.erase(old_peer_xnap_ue_id);
    }

    ue.logger.set_prefix({ue.ue_ids.ue_index, local_xnap_ue_id, peer_xnap_ue_id});
  }

  void update_ue_index(cu_cp_ue_index_t new_ue_index, cu_cp_ue_index_t old_ue_index)
  {
    ocudu_assert(new_ue_index != cu_cp_ue_index_t::invalid, "Invalid new_ue_index={}", new_ue_index);
    ocudu_assert(old_ue_index != cu_cp_ue_index_t::invalid, "Invalid old_ue_index={}", old_ue_index);
    ocudu_assert(ue_index_to_local_xnap_ue_id.find(old_ue_index) != ue_index_to_local_xnap_ue_id.end(),
                 "ue={}: XNAP-UE-ID not found",
                 old_ue_index);

    local_xnap_ue_id_t local_xnap_ue_id = ue_index_to_local_xnap_ue_id.at(old_ue_index);

    ocudu_assert(ues.find(local_xnap_ue_id) != ues.end(),
                 "local_xnap_ue={}: XNAP UE context not found",
                 fmt::underlying(local_xnap_ue_id));

    // Update UE context.
    ues.at(local_xnap_ue_id).ue_ids.ue_index = new_ue_index;

    // Update lookups.
    ue_index_to_local_xnap_ue_id.emplace(new_ue_index, local_xnap_ue_id);
    ue_index_to_local_xnap_ue_id.erase(old_ue_index);

    ues.at(local_xnap_ue_id).logger.set_prefix({ues.at(local_xnap_ue_id).ue_ids.ue_index, local_xnap_ue_id});

    ues.at(local_xnap_ue_id).logger.log_debug("Updated UE index from ue_index={}", old_ue_index);
  }

  void remove_ue_context(cu_cp_ue_index_t ue_index)
  {
    ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);

    if (ue_index_to_local_xnap_ue_id.find(ue_index) == ue_index_to_local_xnap_ue_id.end()) {
      logger.warning("ue={}: XNAP-UE-ID not found", ue_index);
      return;
    }

    // Remove UE from lookup.
    local_xnap_ue_id_t local_xnap_ue_id = ue_index_to_local_xnap_ue_id.at(ue_index);
    ue_index_to_local_xnap_ue_id.erase(ue_index);

    if (ues.find(local_xnap_ue_id) == ues.end()) {
      logger.warning("local_xnap_ue={}: XNAP UE context not found", fmt::underlying(local_xnap_ue_id));
      return;
    }

    ues.at(local_xnap_ue_id).logger.log_debug("Removing XNAP UE context");

    // Parallel CHO candidates of one source share its Source NG-RAN node UE XnAP ID, so the lookup entry for a peer ID
    // may well belong to a sibling context. Only drop the entries that point back at the context being removed.
    if (ues.at(local_xnap_ue_id).ue_ids.peer_xnap_ue_id != peer_xnap_ue_id_t::invalid) {
      erase_peer_xnap_ue_id_lookup(ues.at(local_xnap_ue_id).ue_ids.peer_xnap_ue_id, local_xnap_ue_id);
    }

    for (const auto& [cell, peer_xnap_ue_id] : ues.at(local_xnap_ue_id).cho_prepared) {
      erase_peer_xnap_ue_id_lookup(peer_xnap_ue_id, local_xnap_ue_id);
    }

    ues.erase(local_xnap_ue_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Cancels all transactions pending for all UEs, e.g. as part of XNAP shutdown.
  void cancel_all_transactions()
  {
    for (auto& [id, ue] : ues) {
      ue.cancel_all_transactions();
    }
  }

  /// \brief Get the next available LOCAL_XNAP_UE_ID.
  local_xnap_ue_id_t allocate_local_xnap_ue_id()
  {
    // Return invalid when no LOCAL_XNAP_UE_ID is available.
    if (ue_index_to_local_xnap_ue_id.size() == MAX_NOF_XNAP_UES) {
      return local_xnap_ue_id_t::invalid;
    }

    // Check if the next_local_xnap_ue_id is available.
    if (ues.find(next_local_xnap_ue_id) == ues.end()) {
      local_xnap_ue_id_t ret = next_local_xnap_ue_id;
      // Increase the next LOCAL_XNAP_UE_ID.
      increase_next_local_xnap_ue_id();
      return ret;
    }

    // Iterate over all ids starting with the next_local_xnap_ue_id to find the available id.
    while (true) {
      // Iterate over ue_index_to_local_xnap_ue_id.
      auto it = std::find_if(ue_index_to_local_xnap_ue_id.begin(), ue_index_to_local_xnap_ue_id.end(), [this](auto& u) {
        return u.second == next_local_xnap_ue_id;
      });

      // Return the ID if it is not already used.
      if (it == ue_index_to_local_xnap_ue_id.end()) {
        local_xnap_ue_id_t ret = next_local_xnap_ue_id;
        // Increase the next LOCAL_XNAP_UE_ID.
        increase_next_local_xnap_ue_id();
        return ret;
      }

      // Increase the next LOCAL_XNAP_UE_ID and try again.
      increase_next_local_xnap_ue_id();
    }

    return local_xnap_ue_id_t::invalid;
  }

protected:
  local_xnap_ue_id_t next_local_xnap_ue_id = local_xnap_ue_id_t::min;

private:
  timer_manager&          timers;
  task_executor&          ctrl_exec;
  ocudulog::basic_logger& logger;

  /// \brief Erases the PEER XNAP UE ID lookup entry, but only while it still points at \c local_xnap_ue_id.
  void erase_peer_xnap_ue_id_lookup(peer_xnap_ue_id_t peer_xnap_ue_id, local_xnap_ue_id_t local_xnap_ue_id)
  {
    auto it = peer_xnap_ue_id_to_local_xnap_ue_id.find(peer_xnap_ue_id);
    if (it != peer_xnap_ue_id_to_local_xnap_ue_id.end() and it->second == local_xnap_ue_id) {
      peer_xnap_ue_id_to_local_xnap_ue_id.erase(it);
    }
  }

  void increase_next_local_xnap_ue_id()
  {
    if (next_local_xnap_ue_id == local_xnap_ue_id_t::max) {
      // Reset LOCAL_XNAP_UE_ID counter.
      next_local_xnap_ue_id = local_xnap_ue_id_t::min;
    } else {
      // Increase LOCAL_XNAP_UE_ID counter.
      next_local_xnap_ue_id = uint_to_local_xnap_ue_id(to_underlying(next_local_xnap_ue_id) + 1);
    }
  }

  // Note: Given that UEs will self-remove from the map, we don't want to destructor to clear the lookups beforehand.
  std::unordered_map<cu_cp_ue_index_t, local_xnap_ue_id_t> ue_index_to_local_xnap_ue_id; // indexed by ue_index
  std::unordered_map<peer_xnap_ue_id_t, local_xnap_ue_id_t>
      peer_xnap_ue_id_to_local_xnap_ue_id;                     // indexed by peer_xnap_ue_id_t
  std::unordered_map<local_xnap_ue_id_t, xnap_ue_context> ues; // indexed by local_xnap_ue_id_t
};

} // namespace ocudu::ocucp
