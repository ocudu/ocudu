// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/cell_configuration.h"
#include "../ue_context/ue_repository.h"
#include "configured_grant_scheduler.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

struct cell_slot_resource_allocator;
class uci_allocator;

class configured_grant_scheduler_impl : public configured_grant_scheduler
{
public:
  explicit configured_grant_scheduler_impl(const cell_configuration& cell_cfg_,
                                           uci_allocator&            uci_alloc_,
                                           ue_repository&            ues_);

  void run_slot(cell_resource_allocator& res_alloc) override;

  void rem_ue(const ue_cell_configuration& ue_cfg) override;

  void add_reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration* old_ue_cfg) override;

  /// Called on cell deactivation.
  void stop();

private:
  // Helper to fetch a UE cell by RNTI.
  const ue_cell* get_ue_cell(rnti_t rnti) const;

  // Adds a UE's CG grant positions to the slot wheel. Used by add_reconf_ue().
  void add_ue_to_wheel(const ue_cell_configuration& ue_cfg);

  // Sets the number of reserved CG HARQ processes on the UE cell HARQ entity. Used by add_reconf_ue().
  void update_harq_reservation(const ue_cell_configuration& ue_cfg) const;

  // Processes all CG PUSCH opportunities for the given slot.
  void allocate_slot_cg_opportunities(cell_slot_resource_allocator& slot_alloc) const;

  // Pre-reserves the CG PUSCH RBs and symbols in the UL resource grid for the given slot, for all the UEs with a CG
  // opportunity in that slot, before any dynamic PUSCH grants are allocated in it.
  void reserve_slot_cg_resources(cell_slot_resource_allocator& slot_alloc) const;

  // Pre-reserves the CG PUSCH resources over the whole resource grid for the UEs that were recently added or
  // reconfigured, and clears the list of updated UEs.
  void reserve_updated_ues_resources(cell_resource_allocator& res_alloc);

  // Pre-reserves the CG PUSCH RBs and symbols of a single UE in the UL resource grid for the given slot.
  void reserve_cg_resources(cell_slot_resource_allocator& slot_alloc, const ue_cell& ue_cc) const;

  // Helper that runs preliminary checks on the CG opportunity validity.
  bool validate_cg_opportunity(const cell_slot_resource_allocator& slot_alloc, rnti_t rnti) const;

  // Allocates a single CG PUSCH opportunity for the given RNTI.
  bool allocate_cg_opportunity(cell_slot_resource_allocator& slot_alloc, rnti_t rnti) const;

  pusch_config_params build_cg_pusch_cfg_params(const ue_cell_configuration& ue_cell_cfg) const;

  const cell_configuration& cell_cfg;
  uci_allocator&            uci_alloc;
  ue_repository&            ues;
  ocudulog::basic_logger&   logger;

  // Maximum CG periodicity in slots: sym5120x14 = 5120 slots (normal CP, 14 symbols/slot).
  static constexpr unsigned max_cg_slot_periodicity = 5120U;

  // Storage of the periodic CG PUSCH opportunities to be scheduled in the resource grid. Each position of the vector
  // represents a slot in a ring-like structure (i.e. slot % max_cg_slot_periodicity). Each of these vector
  // indexes/slots contains a list of RNTIs with a CG grant due in that slot.
  circular_vector<static_vector<rnti_t, MAX_PUSCH_PDUS_PER_SLOT>> periodic_pusch_slot_wheel;

  // UEs whose CG configuration has been added or updated in between the last and current slot indications. Their CG
  // resources still need to be pre-reserved over the whole resource grid.
  std::vector<rnti_t> updated_ues;

  // TBS, in bytes, of the CG PUSCH grant of each UE, indexed by the UE's DU index. Insertion and removal do not
  // allocate memory.
  slotted_id_table<du_ue_index_t, units::bytes, MAX_NOF_DU_UES> ue_tbs_values;
};

} // namespace ocudu
