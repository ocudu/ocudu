// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "e1ap_bearer_transaction_manager.h"
#include "e1ap_ue_ids.h"
#include "e1ap_ue_logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include <unordered_map>

namespace ocudu::ocucp {

/// \brief E1AP UE context.
class e1ap_ue_context
{
public:
  e1ap_ue_ids ue_ids;

  // Declare the logger before the transaction manager. Destroying "bearer_ev_mng" cancels its pending
  // transactions, which resumes the awaiting procedures from within this destructor; those procedures resolve
  // the logger through the UE context, so it must still be alive.
  e1ap_ue_logger logger;

  e1ap_bearer_transaction_manager bearer_ev_mng;

  e1ap_ue_context(cu_cp_ue_index_t ue_index_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_, timer_factory timers_) :
    ue_ids({ue_index_, cu_cp_ue_e1ap_id_}), logger("CU-CP-E1", {ue_index_, cu_cp_ue_e1ap_id_}), bearer_ev_mng(timers_)
  {
  }

  const e1ap_ue_ids& get_ue_ids() const { return ue_ids; }

  void update_cu_up_ue_e1ap_id(gnb_cu_up_ue_e1ap_id_t up_ue_id)
  {
    ocudu_assert(up_ue_id != gnb_cu_up_ue_e1ap_id_t::invalid, "Invalid cu_up_ue_e1ap_id={}", fmt::underlying(up_ue_id));
    ue_ids.cu_up_ue_e1ap_id = up_ue_id;

    // Update prefix logger.
    logger.set_prefix({ue_ids.ue_index, ue_ids.cu_cp_ue_e1ap_id, ue_ids.cu_up_ue_e1ap_id});
    logger.log_debug("Updated CU-UP-E1AP-ID");
  }
};

/// \brief List of E1AP UE Contexts in the CU-CP for a given CU-UP.
class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(timer_factory timers_, unsigned max_nof_supported_ues_, ocudulog::basic_logger& logger_) :
    timers(timers_), max_nof_supported_ues(max_nof_supported_ues_), logger(logger_)
  {
  }

  /// \brief Checks whether a UE with the given CU-CP-UE-E1AP-ID exists.
  bool contains(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const { return ues.find(cu_cp_ue_e1ap_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE Index exists.
  /// \param[in] ue_index The UE Index used to find the UE.
  /// \return The UE Index.
  bool contains(cu_cp_ue_index_t ue_index) const
  {
    if (ue_index_to_ue_e1ap_id.find(ue_index) == ue_index_to_ue_e1ap_id.end()) {
      return false;
    }
    if (ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) == ues.end()) {
      logger.warning("No UE context found for cu_cp_ue_e1ap_id={}",
                     fmt::underlying(ue_index_to_ue_e1ap_id.at(ue_index)));
      return false;
    }
    return true;
  }

  /// \brief Search for a UE based on its CU-CP-UE-E1AP-ID.
  const e1ap_ue_context* find_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const
  {
    auto it = ues.find(cu_cp_ue_e1ap_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
  }
  e1ap_ue_context* find_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    auto it = ues.find(cu_cp_ue_e1ap_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
  }

  e1ap_ue_context* find_ue(cu_cp_ue_index_t ue_idx)
  {
    auto it = ue_index_to_ue_e1ap_id.find(ue_idx);
    return it != ue_index_to_ue_e1ap_id.end() ? &ues.at(it->second) : nullptr;
  }

  e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    ocudu_assert(ues.find(cu_cp_ue_e1ap_id) != ues.end(),
                 "cu_cp_ue_e1ap_id={}: E1AP UE context not found",
                 fmt::underlying(cu_cp_ue_e1ap_id));
    return ues.at(cu_cp_ue_e1ap_id);
  }
  e1ap_ue_context& operator[](cu_cp_ue_index_t ue_index)
  {
    ocudu_assert(ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end(),
                 "ue={} gNB-CU-CP-UE-E1AP-ID not found",
                 fmt::underlying(ue_index));
    ocudu_assert(ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) != ues.end(),
                 "cu_cp_ue_e1ap_id={}: E1AP UE context not found",
                 fmt::underlying(ue_index_to_ue_e1ap_id.at(ue_index)));
    return ues.at(ue_index_to_ue_e1ap_id.at(ue_index));
  }

  /// \brief Create new UE E1AP Context.
  e1ap_ue_context* add_ue(cu_cp_ue_index_t ue_index, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id);

  /// Remove existing E1AP UE Context.
  void remove_ue(cu_cp_ue_index_t ue_index);

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t allocate_gnb_cu_cp_ue_e1ap_id();

  /// \brief Transfer E1AP UE context to new CU-CP specific UE index.
  void update_ue_index(cu_cp_ue_index_t new_ue_index, cu_cp_ue_index_t old_ue_index);

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>::iterator       begin() { return ues.begin(); }
  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>::const_iterator begin() const { return ues.begin(); }
  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>::iterator       end() { return ues.end(); }
  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>::const_iterator end() const { return ues.end(); }

private:
  gnb_cu_cp_ue_e1ap_id_t next_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::min;

  timer_factory           timers;
  unsigned                max_nof_supported_ues;
  ocudulog::basic_logger& logger;

  void increase_next_cu_cp_ue_e1ap_id();

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>  ues; // indexed by gnb_cu_cp_ue_e1ap_id
  std::unordered_map<cu_cp_ue_index_t, gnb_cu_cp_ue_e1ap_id_t> ue_index_to_ue_e1ap_id; // indexed by ue_index
};

} // namespace ocudu::ocucp
