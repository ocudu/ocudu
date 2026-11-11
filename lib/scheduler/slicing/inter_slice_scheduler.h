// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../policy/scheduler_policy.h"
#include "ran_slice_candidate.h"
#include "ran_slice_instance.h"
#include "ocudu/adt/slotted_vector.h"
#include <memory>
#include <vector>

namespace ocudu {

/// \brief [Implementation-defined] Maximum number of distinct Koffsets a RAN slice generates UL candidates for.
///
/// UEs on other Koffsets are only scheduled in slots that a listed Koffset reaches.
constexpr unsigned MAX_NOF_NTN_KOFFSETS_PER_SLICE = 4;

struct cell_resource_allocator;

/// Inter-slice Scheduler.
class inter_slice_scheduler
{
  using priority_type                      = uint32_t;
  using slot_count_type                    = uint32_t;
  static constexpr priority_type skip_prio = 0;

  /// Maximum number of RAN slices. The last ID is the logical channel system's "no slice" marker.
  static constexpr unsigned MAX_NOF_RAN_SLICES = MAX_NOF_RAN_SLICE_IDS - 1;

public:
  inter_slice_scheduler(const cell_configuration& cell_cfg_, ue_repository& ues_);

  /// Reset the state of the slices.
  void slot_indication(slot_point slot_tx, const cell_resource_allocator& res_grid);

  /// Update the state of the slice with the provided UE configs.
  void add_ue(du_ue_index_t ue_idx);
  void reconf_ue(du_ue_index_t ue_idx);
  void rem_ue(du_ue_index_t ue_idx);
  void config_applied(du_ue_index_t ue_idx);

  /// Get next RAN slice for PDSCH scheduling.
  std::optional<dl_ran_slice_candidate> get_next_dl_candidate();

  /// Get next RAN slice for PUSCH scheduling.
  std::optional<ul_ran_slice_candidate> get_next_ul_candidate();

  size_t nof_slices() const { return slices.size(); }

  /// Returns the number of RAN slice IDs up to the highest one in use. The IDs of the slices are stable, so removing a
  /// slice leaves a hole.
  size_t nof_slice_ids() const { return slice_id_range; }

  const slice_rrm_policy_config& slice_config(ran_slice_id_t id) const { return slices[id]->inst.cfg; }
  scheduler_policy&              get_policy(ran_slice_id_t id) const { return *slices[id]->policy; }

  /// Whether the slice was created at runtime, and so it can be removed at runtime.
  bool is_dynamic(ran_slice_id_t id) const { return slices[id]->is_dynamic; }

  /// \brief Applies new RB limits to slices, and creates and removes slices at runtime.
  ///
  /// The UEs of a removed slice move to the default DRB slice, and the RBs of the slice go back to the shared pool. The
  /// HARQ retransmissions pending for the removed slice are cancelled. A slice created at runtime has no UEs until
  /// they are reconfigured. The RAN slice IDs of the other slices do not change.
  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req);

private:
  /// Creates a slice at runtime. The slice is ignored if the policy member already has one, or if its dedicated RBs do
  /// not fit in the cell next to those of the other slices.
  void create_slice(const du_cell_slice_reconfig_request::rrm_policy_config& new_slice);

  /// Removes a slice created at runtime. The slice is ignored if it is not known or was not created at runtime.
  void remove_slice(const rrm_policy_member& member);

  /// Returns the total number of RBs that are reserved for dedicated RBs + those that have been already allocated to
  /// other slices.
  template <bool IsDownlink>
  unsigned get_nof_used_reserved_rbs(slot_point slot_tx) const;

  /// Class responsible for tracking the scheduling context of each RAN slice instance.
  struct ran_slice_sched_context {
    ran_slice_instance                inst;
    std::unique_ptr<scheduler_policy> policy;
    /// Whether the slice was created at runtime. Only such a slice can be removed at runtime.
    bool is_dynamic;

    ran_slice_sched_context(ran_slice_id_t                    id,
                            const cell_configuration&         cell_cfg,
                            const slice_rrm_policy_config&    cfg,
                            std::unique_ptr<scheduler_policy> policy_,
                            ue_repository&                    ues_,
                            bool                              is_dynamic_ = false) :
      inst(id, cell_cfg, cfg, ues_), policy(std::move(policy_)), is_dynamic(is_dynamic_)
    {
    }

    /// Determines the slice candidate priority.
    priority_type get_prio(bool is_dl, slot_point pdcch_slot, slot_point pxsch_slot, unsigned rb_lims) const;
  };

  struct slice_candidate_context {
    ran_slice_id_t id;
    priority_type  prio;
    /// Range of RBs within which this slice candidate is valid.
    interval<unsigned> rb_lims;
    /// Slot at which PUSCH/PDSCH needs to be scheduled for this slice candidate.
    slot_point slot_tx;
    /// k2 of the PUSCH TD resource this candidate was built from. Zero for downlink.
    unsigned k2;
    /// Koffset in slots of the UEs this candidate is for, as per TS 38.213, Section 4.2. Zero for downlink.
    unsigned ntn_koffset;

    slice_candidate_context(ran_slice_id_t     id_,
                            priority_type      prio_,
                            interval<unsigned> rb_lims_,
                            slot_point         slot_tx_,
                            unsigned           k2_          = 0,
                            unsigned           ntn_koffset_ = 0) :
      id(id_), prio(prio_), rb_lims(rb_lims_), slot_tx(slot_tx_), k2(k2_), ntn_koffset(ntn_koffset_)
    {
    }

    /// Compares priorities between two slice contexts.
    bool operator<(const slice_candidate_context& rhs) const { return prio < rhs.prio; }
    bool operator>(const slice_candidate_context& rhs) const { return prio > rhs.prio; }
  };

  class slice_prio_queue
  {
  public:
    void reserve(size_t cap) { queue.reserve(cap); }

    void push(const slice_candidate_context& candidate)
    {
      if (candidate.prio == skip_prio) {
        return;
      }
      queue.push_back(candidate);
    }

    void sort() { std::sort(queue.begin(), queue.end(), std::greater<slice_candidate_context>{}); }

    const slice_candidate_context& top() const { return queue[next_pop]; }

    void pop() { next_pop++; }

    bool empty() const { return next_pop >= queue.size(); }

    void clear()
    {
      next_pop = 0;
      queue.clear();
    }

  private:
    std::vector<slice_candidate_context> queue;
    unsigned                             next_pop = 0;
  };

  ran_slice_instance& get_slice(const logical_channel_config& lc_cfg);

  // Fetch UE if it is in a state to be added/reconfigured.
  ue* fetch_ue_to_update(du_ue_index_t ue_idx) const;

  /// \brief Adds the UL candidates of a slice for a PUSCH TD resource reached at the given Koffset.
  /// \param[in,out] allocated_k2 Last k2 given a candidate at this Koffset. Set to \c td_res.k2 when one is added.
  /// \param[in]     slice        Slice the candidates are for.
  /// \param[in]     slot_tx      Slot of the PDCCH.
  /// \param[in]     res_grid     Cell resource grid.
  /// \param[in]     td_res       PUSCH TD resource the candidates are built from.
  /// \param[in]     ntn_koffset  Koffset in slots of the UEs the candidates are for.
  void add_ul_slice_candidates(std::optional<unsigned>&                     allocated_k2,
                               const ran_slice_sched_context&               slice,
                               slot_point                                   slot_tx,
                               const cell_resource_allocator&               res_grid,
                               const pusch_time_domain_resource_allocation& td_res,
                               unsigned                                     ntn_koffset);

  void add_impl(ue& u);

  template <bool IsDownlink>
  std::optional<std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>> get_next_candidate();

  const cell_configuration& cell_cfg;
  ocudulog::basic_logger&   logger;

  // Represents current slot in the scheduler. This is updated on each slot indication.
  slot_point current_slot;

  ue_repository& ues;

  // The slices are indexed by RAN slice ID. The ID of a slice does not change while it exists, as the HARQ processes
  // keep the ID they were sent for.
  slotted_id_vector<ran_slice_id_t, std::unique_ptr<ran_slice_sched_context>> slices;

  // Highest RAN slice ID in use plus one.
  size_t slice_id_range = 0;

  // Queue of slice candidates sorted by priority.
  slice_prio_queue dl_prio_queue;
  slice_prio_queue ul_prio_queue;
};

} // namespace ocudu
