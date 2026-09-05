// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "nrppa_ue_context.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;
using namespace ocucp;

nrppa_ue_context_list::nrppa_ue_context_list(ocudulog::basic_logger& logger_) : logger(logger_) {}

bool nrppa_ue_context_list::contains(cu_cp_ue_index_t ue_index) const
{
  return ues.find(ue_index) != ues.end();
}

nrppa_ue_context& nrppa_ue_context_list::operator[](cu_cp_ue_index_t ue_index)
{
  ocudu_assert(ues.find(ue_index) != ues.end(), "ue={}: NRPPA UE context not found", ue_index);

  return ues.at(ue_index);
}

nrppa_ue_context& nrppa_ue_context_list::add_ue(cu_cp_ue_index_t         ue_index,
                                                ran_ue_meas_id_t         ran_ue_meas_id,
                                                lmf_ue_meas_id_t         lmf_ue_meas_id,
                                                nrppa_cu_cp_ue_notifier& ue_notifier,
                                                timer_manager&           timers,
                                                task_executor&           task_exec)
{
  logger.debug("ue={} ran_ue={} lmf_ue={}: NRPPA UE context created",
               fmt::underlying(ue_index),
               fmt::underlying(ran_ue_meas_id),
               fmt::underlying(lmf_ue_meas_id));
  ues.emplace(std::piecewise_construct,
              std::forward_as_tuple(ue_index),
              std::forward_as_tuple(ue_index, ran_ue_meas_id, lmf_ue_meas_id, ue_notifier, timers, task_exec));
  return ues.at(ue_index);
}

void nrppa_ue_context_list::update_ue_index(cu_cp_ue_index_t         new_ue_index,
                                            cu_cp_ue_index_t         old_ue_index,
                                            nrppa_cu_cp_ue_notifier& new_ue_notifier,
                                            timer_manager&           timers,
                                            task_executor&           task_exec)
{
  ocudu_assert(new_ue_index != cu_cp_ue_index_t::invalid, "Invalid new_ue_index={}", new_ue_index);
  ocudu_assert(old_ue_index != cu_cp_ue_index_t::invalid, "Invalid old_ue_index={}", old_ue_index);
  ocudu_assert(ues.find(old_ue_index) != ues.end(), "ue={}: NRPPA UE context not found", old_ue_index);
  ocudu_assert(ues.find(new_ue_index) == ues.end(), "ue={}: NRPPA UE context already exists", new_ue_index);

  nrppa_ue_ids ue_ids = ues.at(old_ue_index).ue_ids;

  // Create new UE context.
  ues.emplace(std::piecewise_construct,
              std::forward_as_tuple(new_ue_index),
              std::forward_as_tuple(
                  new_ue_index, ue_ids.ran_ue_meas_id, ue_ids.lmf_ue_meas_id, new_ue_notifier, timers, task_exec));

  // Remove old UE context.
  ues.erase(old_ue_index);

  ues.at(new_ue_index).logger.log_debug("Updated UE index from ue_index={}", old_ue_index);
}

void nrppa_ue_context_list::remove_ue_context(cu_cp_ue_index_t ue_index)
{
  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: NRPPA UE not found", ue_index);
    return;
  }

  ues.at(ue_index).logger.log_debug("Removing NRPPA UE context");
  ues.erase(ue_index);
}

size_t nrppa_ue_context_list::size() const
{
  return ues.size();
}

/// \brief Get the next available RAN UE meas ID.
expected<ran_ue_meas_id_t, std::string> nrppa_ue_context_list::allocate_ran_ue_meas_id()
{
  // Return false when no RAN UE meas ID is available.
  if (ues.size() == (unsigned)ran_ue_meas_id_t::max) {
    return make_unexpected(fmt::format("Maxium number of RAN UE MEAS IDs reached"));
  }

  // Iterate over all IDs starting with the next_ran_ue_meas_id to find the available ID.
  while (true) {
    // Iterate over UEs.
    auto it = std::find_if(
        ues.begin(), ues.end(), [this](auto& u) { return u.second.ue_ids.ran_ue_meas_id == next_ran_ue_meas_id; });

    // Return the ID if it is not already used.
    if (it == ues.end()) {
      ran_ue_meas_id_t ret = next_ran_ue_meas_id;
      // Increase the next RAN UE meas ID.
      increase_next_ran_ue_meas_id();
      return ret;
    }

    // Increase the next RAN UE meas ID and try again.
    increase_next_ran_ue_meas_id();
  }

  return make_unexpected(fmt::format("Maxium number of RAN UE MEAS IDs reached"));
}

void nrppa_ue_context_list::increase_next_ran_ue_meas_id()
{
  if (next_ran_ue_meas_id == ran_ue_meas_id_t::max) {
    // Reset RAN UE meas ID counter.
    next_ran_ue_meas_id = ran_ue_meas_id_t::min;
  } else {
    // Increase RAN UE meas ID counter.
    next_ran_ue_meas_id = uint_to_ran_ue_meas_id(to_underlying(next_ran_ue_meas_id) + 1);
  }
}
