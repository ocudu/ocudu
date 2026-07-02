// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "xnap_ue_logger.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/support/timers.h"
#include "ocudu/xnap/xnap_types.h"

namespace ocudu::ocucp {

struct xnap_ue_ids {
  cu_cp_ue_index_t         ue_index         = cu_cp_ue_index_t::invalid;
  const local_xnap_ue_id_t local_xnap_ue_id = local_xnap_ue_id_t::invalid;
  peer_xnap_ue_id_t        peer_xnap_ue_id  = peer_xnap_ue_id_t::invalid;
};

struct xnap_ue_context {
  xnap_ue_ids    ue_ids;
  xnap_ue_logger logger;

  /// XN Handover Request Ack/Handover Preparation Failure Event Source.
  protocol_transaction_event_source<asn1::xnap::ho_request_ack_s, asn1::xnap::ho_prep_fail_s> xn_handover_outcome;

  /// XN Status Transfer Event Source.
  protocol_transaction_event_source<asn1::xnap::sn_status_transfer_s> sn_status_transfer_outcome;

  xnap_ue_context(cu_cp_ue_index_t        ue_index_,
                  local_xnap_ue_id_t      local_xnap_ue_id_,
                  timer_factory           timer_db,
                  ocudulog::basic_logger& logger_) :
    ue_ids({ue_index_, local_xnap_ue_id_}),
    logger("XNAP", {ue_index_, local_xnap_ue_id_}),
    xn_handover_outcome(timer_db),
    sn_status_transfer_outcome(timer_db)
  {
  }

  /// \brief Cancels all transactions pending for this UE, e.g. as part of XNAP shutdown.
  void cancel_all_transactions()
  {
    xn_handover_outcome.stop();
    sn_status_transfer_outcome.stop();
  }
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

    if (ues.at(local_xnap_ue_id).ue_ids.peer_xnap_ue_id != peer_xnap_ue_id_t::invalid) {
      peer_xnap_ue_id_to_local_xnap_ue_id.erase(ues.at(local_xnap_ue_id).ue_ids.peer_xnap_ue_id);
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

  void increase_next_local_xnap_ue_id()
  {
    if (next_local_xnap_ue_id == local_xnap_ue_id_t::max) {
      // Reset LOCAL_XNAP_UE_ID counter.
      next_local_xnap_ue_id = local_xnap_ue_id_t::min;
    } else {
      // Increase LOCAL_XNAP_UE_ID counter.
      next_local_xnap_ue_id = uint_to_local_xnap_ue_id(local_xnap_ue_id_to_uint(next_local_xnap_ue_id) + 1);
    }
  }

  // Note: Given that UEs will self-remove from the map, we don't want to destructor to clear the lookups beforehand.
  std::unordered_map<cu_cp_ue_index_t, local_xnap_ue_id_t> ue_index_to_local_xnap_ue_id; // indexed by ue_index
  std::unordered_map<peer_xnap_ue_id_t, local_xnap_ue_id_t>
      peer_xnap_ue_id_to_local_xnap_ue_id;                     // indexed by peer_xnap_ue_id_t
  std::unordered_map<local_xnap_ue_id_t, xnap_ue_context> ues; // indexed by local_xnap_ue_id_t
};

} // namespace ocudu::ocucp
