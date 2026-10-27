// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "f1ap_cu_ue_transaction_manager.h"
#include "f1ap_ue_logger.h"
#include "ue_ul_bearer_manager.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/f1ap/cu_cp/gnb_cu_ue_f1ap_id_allocator.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/support/async/manual_event.h"
#include <unordered_map>

namespace ocudu::ocucp {

struct f1ap_ue_context {
  f1ap_ue_ids ue_ids;
  /// Whether the release procedure has been started for this UE context.
  bool marked_for_release = false;
  /// Whether the F1 TNL is being torn down for this UE (no more F1AP Tx/Rx expected). When set, the F1AP UE Context
  /// Release Procedure skips sending the UEContextReleaseCommand and returns success immediately. The DU will drop
  /// the context locally as part of the F1 removal.
  bool f1_removal_in_progress = false;
  /// Event to notify all pending release callers when release completes. Stores the result.
  manual_event<cu_cp_ue_index_t> release_complete_event;
  /// Whether the old gNB-DU UE F1AP UE ID IE needs to be notified back to the DU, due to reestablishment.
  std::optional<gnb_du_ue_f1ap_id_t> pending_old_ue_id;
  f1ap_ue_transaction_manager        ev_mng;
  f1ap_ue_logger                     logger;

  f1ap_ue_context(cu_cp_ue_index_t ue_index_, gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id_, timer_factory timers_) :
    ue_ids({ue_index_, cu_ue_f1ap_id_}), ev_mng(timers_), logger("CU-CP-F1", {ue_ids}, ": ")
  {
  }

  /// Returns the instance of the Uplink bearer manager of this UE.
  ue_ul_bearer_manager&       get_ul_bearer_manager() { return bearer_manager; }
  const ue_ul_bearer_manager& get_ul_bearer_manager() const { return bearer_manager; }

  /// Checks whether a release procedure runs for this UE context.
  bool is_release_running() const { return marked_for_release and not release_complete_event.is_set(); }

  /// Handles Downlink RRC messages.
  void handle_dl_rrc_message(const f1ap_dl_rrc_message& msg, f1ap_message_notifier& msg_notifier);

private:
  ue_ul_bearer_manager bearer_manager;
};

class f1ap_ue_context_list
{
public:
  f1ap_ue_context_list(gnb_cu_ue_f1ap_id_allocator& cu_ue_id_alloc_,
                       timer_factory                timers_,
                       ocudulog::basic_logger&      logger_) :
    cu_ue_id_alloc(cu_ue_id_alloc_), timers(timers_), logger(logger_)
  {
  }

  ~f1ap_ue_context_list();

  bool contains(gnb_cu_ue_f1ap_id_t cu_ue_id) const { return ues.find(cu_ue_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return The CU UE ID.
  bool contains(cu_cp_ue_index_t ue_index) const;

  f1ap_ue_context& operator[](gnb_cu_ue_f1ap_id_t cu_ue_id);
  f1ap_ue_context& operator[](cu_cp_ue_index_t ue_index);

  const f1ap_ue_context* find(gnb_du_ue_f1ap_id_t du_ue_id) const;

  const f1ap_ue_context* find(gnb_cu_ue_f1ap_id_t cu_ue_id) const;
  f1ap_ue_context*       find(gnb_cu_ue_f1ap_id_t cu_ue_id);

  const f1ap_ue_context* find(cu_cp_ue_index_t ue_idx) const;
  f1ap_ue_context*       find(cu_cp_ue_index_t ue_idx);

  f1ap_ue_context& add_ue(cu_cp_ue_index_t ue_index, gnb_cu_ue_f1ap_id_t cu_ue_id);

  void add_du_ue_f1ap_id(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

  /// \brief Gives a UE context to another UE index.
  ///
  /// An RRC Resume uses a new UE-associated logical F1-connection (TS 38.401 section 8.6.2). Thus, the UE context of
  /// the new F1-connection replaces the UE context that the UE index had before. The replaced UE context is removed
  /// now if its release has completed. If its release is running, the release procedure removes it at the end.
  /// Otherwise, remove_ue() removes it together with the UE.
  void update_ue_index(gnb_cu_ue_f1ap_id_t cu_ue_id, cu_cp_ue_index_t ue_index);

  /// Checks whether a UE context was replaced by another UE context of the same UE index.
  bool is_replaced(gnb_cu_ue_f1ap_id_t cu_ue_id) const;

  /// \brief Removes the UE contexts of a UE.
  ///
  /// A replaced UE context with a running release stays. Its release procedure removes it at the end.
  void remove_ue(cu_cp_ue_index_t ue_index);

  /// Removes a UE context.
  void remove_ue(gnb_cu_ue_f1ap_id_t cu_ue_id);

  /// \brief Cancels all running F1AP transactions of a UE context.
  ///
  /// Each cancellation resumes the waiting procedure in-line. That procedure, or the routine that started it, can
  /// remove the F1AP UE context. Thus, this function finds the UE context again before each cancellation.
  void cancel_ue_transactions(gnb_cu_ue_f1ap_id_t cu_ue_id);

  void add_srb0_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_ccch_notifier* srb0_notifier);
  void add_srb1_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_dcch_notifier* srb1_notifier);
  void add_srb2_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_dcch_notifier* srb2_notifier);

  size_t size() const { return ues.size(); }

  /// Allocates a gNB-CU UE F1AP ID that is unique within the CU-CP.
  /// \return The allocated ID, or gnb_cu_ue_f1ap_id_t::invalid if all IDs are in use.
  gnb_cu_ue_f1ap_id_t allocate_gnb_cu_ue_f1ap_id() { return cu_ue_id_alloc.allocate(); }

  /// Releases a gNB-CU UE F1AP ID that no UE context uses.
  void release_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id_t cu_ue_id) { cu_ue_id_alloc.release(cu_ue_id); }

  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::iterator       begin() { return ues.begin(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::const_iterator begin() const { return ues.begin(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::iterator       end() { return ues.end(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::const_iterator end() const { return ues.end(); }

private:
  gnb_cu_ue_f1ap_id_allocator& cu_ue_id_alloc;
  timer_factory                timers;
  ocudulog::basic_logger&      logger;

  // Note: Given that UEs will self-remove from the map, we don't want to destructor to clear the lookups beforehand.
  std::unordered_map<cu_cp_ue_index_t, gnb_cu_ue_f1ap_id_t> ue_index_to_ue_f1ap_id; // indexed by ue_index
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>  ues;                    // indexed by gnb_cu_ue_f1ap_id
};

} // namespace ocudu::ocucp
