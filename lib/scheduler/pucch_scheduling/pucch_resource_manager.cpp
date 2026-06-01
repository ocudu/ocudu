// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_resource_manager.h"
#include "pucch_collision_manager.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

/////////////    RESOURCE MANAGER     /////////////

/////////////   Public methods   /////////////

pucch_resource_manager::pucch_resource_manager(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_), cell_resources(cell_cfg_.bwp_res[to_bwp_id(0)].ul().pucch), collision_manager(cell_cfg_)
{
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  collision_manager.slot_indication(slot_tx);
}

void pucch_resource_manager::stop()
{
  collision_manager.stop();
}

bool pucch_resource_manager::reserve_harq_common_resource(cell_slot_resource_allocator& slot_alloc,
                                                          size_t                        r_pucch,
                                                          rnti_t                        rnti)

{
  return collision_manager.alloc(slot_alloc, cell_resources.get_cmn(r_pucch), rnti).has_value();
}

void pucch_resource_manager::release_harq_common_resource(cell_slot_resource_allocator& slot_alloc,
                                                          size_t                        r_pucch,
                                                          rnti_t                        rnti)
{
  collision_manager.free(slot_alloc, cell_resources.get_cmn(r_pucch), rnti);
}

/////////////   UE Reservation Guard   /////////////

pucch_resource_manager::ue_reservation_guard::ue_reservation_guard(pucch_resource_manager*       parent_,
                                                                   cell_slot_resource_allocator& slot_alloc_,
                                                                   rnti_t                        rnti_,
                                                                   const ue_cell_configuration&  ue_cfg_) :
  parent(parent_),
  slot_alloc(slot_alloc_),
  res_params(parent_->cell_cfg.params.init_bwp.pucch.resources),
  cell_resources(parent_->cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch),
  rnti(rnti_),
  ue_bwp_cfg(*ue_cfg_.bwp(to_bwp_id(0)).ul.ue_cfg())
{
  ocudu_sanity_check(parent != nullptr, "PUCCH Resource Manager pointer cannot be null");
  ocudu_sanity_check(rnti != rnti_t::INVALID_RNTI, "RNTI cannot be invalid");
}

pucch_resource_manager::ue_reservation_guard::~ue_reservation_guard()
{
  rollback();
}

pucch_harq_resource_alloc_record
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_0_resource_next_available()
{
  return reserve_next_harq_res_available<0>();
}

pucch_harq_resource_alloc_record
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_1_resource_next_available()
{
  return reserve_next_harq_res_available<1>();
}

const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_0_resource_by_res_indicator(unsigned d_pri)

{
  return reserve_harq_resource_by_res_indicator<0>(d_pri);
}

const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_1_resource_by_res_indicator(unsigned d_pri)
{
  return reserve_harq_resource_by_res_indicator<1>(d_pri);
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::reserve_sr_resource()
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  const auto& sr_res    = cell_resources.get_ded(res_params.sr_res_id(ue_bwp_cfg.pucch.sr_res_id));
  const auto  alloc_res = parent->collision_manager.alloc(slot_alloc, sr_res, rnti);
  if (not alloc_res.has_value()) {
    if (alloc_res.error() == pucch_collision_manager::alloc_failure_reason::ALREADY_ALLOCATED) {
      // Resource is already allocated to this RNTI, just return it.
      return &sr_res;
    }
    return nullptr;
  }

  const unsigned sr_cell_res_id                                = sr_res.res_id.ded().cell_res_id;
  reservations[static_cast<unsigned>(resource_usage_type::sr)] = {sr_cell_res_id};
  return &sr_res;
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::reserve_csi_resource()
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  const auto& csi_res   = cell_resources.get_ded(res_params.csi_res_id(ue_bwp_cfg.periodic_csi_report->pucch_res_id));
  const auto  alloc_res = parent->collision_manager.alloc(slot_alloc, csi_res, rnti);
  if (not alloc_res.has_value()) {
    if (alloc_res.error() == pucch_collision_manager::alloc_failure_reason::ALREADY_ALLOCATED) {
      // Resource is already allocated to this RNTI, just return it.
      return &csi_res;
    }
    return nullptr;
  }

  const unsigned csi_cell_res_id                                = csi_res.res_id.ded().cell_res_id;
  reservations[static_cast<unsigned>(resource_usage_type::csi)] = {csi_cell_res_id};
  return &csi_res;
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::peek_sr_resource() const
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  const auto&    sr_res         = cell_resources.get_ded(res_params.sr_res_id(ue_bwp_cfg.pucch.sr_res_id));
  const unsigned sr_cell_res_id = sr_res.res_id.ded().cell_res_id;
  return &cell_resources.dedicated[sr_cell_res_id];
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::peek_csi_resource() const
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  const auto&    csi_res = cell_resources.get_ded(res_params.csi_res_id(ue_bwp_cfg.periodic_csi_report->pucch_res_id));
  const unsigned csi_cell_res_id = csi_res.res_id.ded().cell_res_id;
  return &cell_resources.dedicated[csi_cell_res_id];
}

bool pucch_resource_manager::ue_reservation_guard::release_harq_set_0_resource()
{
  return release_harq_resource<0>();
}

bool pucch_resource_manager::ue_reservation_guard::release_harq_set_1_resource()
{
  return release_harq_resource<1>();
}

bool pucch_resource_manager::ue_reservation_guard::release_sr_resource()
{
  ocudu_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");

  const auto& sr_res = cell_resources.get_ded(res_params.sr_res_id(ue_bwp_cfg.pucch.sr_res_id));
  if (not parent->collision_manager.free(slot_alloc, sr_res, rnti)) {
    return false;
  }

  reservations[static_cast<unsigned>(resource_usage_type::sr)].cell_res_id = std::nullopt;
  return true;
}

bool pucch_resource_manager::ue_reservation_guard::release_csi_resource()
{
  ocudu_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");

  const auto& csi_res = cell_resources.get_ded(res_params.csi_res_id(ue_bwp_cfg.periodic_csi_report->pucch_res_id));
  if (not parent->collision_manager.free(slot_alloc, csi_res, rnti)) {
    return false;
  }

  reservations[static_cast<unsigned>(resource_usage_type::sr)].cell_res_id = std::nullopt;
  return true;
}

template <unsigned ResourceSetId>
pucch_harq_resource_alloc_record pucch_resource_manager::ue_reservation_guard::reserve_next_harq_res_available()
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  // If a resource is already allocated to this RNTI, use it.
  std::optional<unsigned> available_res;
  const auto              res_set_cfg_id = ue_bwp_cfg.pucch.res_set_cfg_id;
  const unsigned          res_set_size   = res_params.res_set_size.value();
  for (uint8_t r_pucch = 0; r_pucch != res_set_size; ++r_pucch) {
    const auto& res       = cell_resources.get_ded(res_params.harq_res_id<ResourceSetId>(res_set_cfg_id, r_pucch));
    const auto  alloc_res = parent->collision_manager.can_alloc(slot_alloc, res, rnti);
    if (not alloc_res.has_value() and
        alloc_res.error() == pucch_collision_manager::alloc_failure_reason::ALREADY_ALLOCATED) {
      available_res = r_pucch;
      break;
    }
  }

  if (not available_res.has_value()) {
    // Else, try to allocate the first available resource.
    for (unsigned r_pucch = 0; r_pucch != res_set_size; ++r_pucch) {
      const auto& res = cell_resources.get_ded(res_params.harq_res_id<ResourceSetId>(res_set_cfg_id, r_pucch));
      if (parent->collision_manager.alloc(slot_alloc, res, rnti).has_value()) {
        const unsigned usage_type_idx = ResourceSetId == 0 ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                                           : static_cast<unsigned>(resource_usage_type::harq_set_1);
        unsigned       cell_res_id    = res.res_id.ded().cell_res_id;
        reservations[usage_type_idx]  = {cell_res_id};
        available_res                 = r_pucch;
        break;
      }
    }
  }

  // If an available resource was found, return it.
  if (available_res.has_value()) {
    const auto&    res = cell_resources.get_ded(res_params.harq_res_id<ResourceSetId>(res_set_cfg_id, *available_res));
    const unsigned cell_res_id = res.res_id.ded().cell_res_id;
    return pucch_harq_resource_alloc_record{.resource            = &cell_resources.dedicated[cell_res_id],
                                            .pucch_res_indicator = static_cast<uint8_t>(available_res.value())};
  }
  return pucch_harq_resource_alloc_record{.resource = nullptr};
}

template <unsigned ResourceSetId>
const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_resource_by_res_indicator(unsigned d_pri)
{
  ocudu_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");
  const unsigned res_set_size = res_params.res_set_size.value();
  const unsigned max_pri      = parent->cell_cfg.is_pucch_f0_and_f2()
                                    ? res_set_size + (ue_bwp_cfg.periodic_csi_report.has_value() ? 2U : 1U)
                                    : res_set_size;
  // Make sure the resource indicator points to a valid resource.
  if (d_pri >= max_pri) {
    return nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  // [Implementation-defined] We assume at most 8 resources per resource set. If this is the case, r_pucch = d_pri.

  // For Format 0 and Format 2, the resources indexed by PUCCH res. indicators >= res_set_size are reserved for CSI and
  // SR slots. In the case, we don't need to reserve these in the PUCCH resource manager, we only need to return the
  // resources.
  if (parent->cell_cfg.is_pucch_f0_and_f2() and d_pri >= res_set_size) {
    if (ResourceSetId == 0) {
      if (d_pri == res_set_size) {
        return &cell_resources.get_ded(res_params.sr_res_id(ue_bwp_cfg.pucch.sr_res_id));
      }
      return &cell_resources.get_ded(res_params.csi_f0_res_id(ue_bwp_cfg.periodic_csi_report->pucch_res_id));
    }
    // Resource Set ID 1.
    if (d_pri == res_set_size) {
      return &cell_resources.get_ded(res_params.sr_f2_res_id(ue_bwp_cfg.pucch.sr_res_id));
    }
    return &cell_resources.get_ded(res_params.csi_res_id(ue_bwp_cfg.periodic_csi_report->pucch_res_id));
  }

  const auto& res =
      cell_resources.get_ded(res_params.harq_res_id<ResourceSetId>(ue_bwp_cfg.pucch.res_set_cfg_id, d_pri));
  const auto alloc_res = parent->collision_manager.alloc(slot_alloc, res, rnti);
  if (not alloc_res.has_value()) {
    if (alloc_res.error() == pucch_collision_manager::alloc_failure_reason::ALREADY_ALLOCATED) {
      // Resource is already allocated to this RNTI, just return it.
      return &res;
    }
    return nullptr;
  }

  const unsigned usage_type_idx = ResourceSetId == 0 ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                                     : static_cast<unsigned>(resource_usage_type::harq_set_1);
  const unsigned cell_res_id    = res.res_id.ded().cell_res_id;
  reservations[usage_type_idx]  = {cell_res_id};
  return &cell_resources.dedicated[cell_res_id];
}

template <unsigned ResourceSetId>
bool pucch_resource_manager::ue_reservation_guard::release_harq_resource()
{
  ocudu_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");

  const unsigned res_set_size = res_params.res_set_size.value();

  for (unsigned r_pucch = 0; r_pucch != res_set_size; ++r_pucch) {
    const auto& res =
        cell_resources.get_ded(res_params.harq_res_id<ResourceSetId>(ue_bwp_cfg.pucch.res_set_cfg_id, r_pucch));
    if (parent->collision_manager.free(slot_alloc, res, rnti)) {
      // Release the resource.
      const unsigned usage_type_idx = ResourceSetId == 0 ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                                         : static_cast<unsigned>(resource_usage_type::harq_set_1);
      reservations[usage_type_idx].cell_res_id = std::nullopt;
      return true;
    }
  }

  // If no resource could be released, return false.
  return false;
}

void pucch_resource_manager::ue_reservation_guard::commit()
{
  ocudu_assert(parent != nullptr, "Trying to commit PUCCH resource reservations after commit has been called");

  // If both HARQ resource sets have been reserved, only keep the resource in PUCCH Resource Set ID 1.
  if (reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id.has_value() and
      reservations[static_cast<unsigned>(resource_usage_type::harq_set_1)].cell_res_id.has_value()) {
    // Release HARQ Resource Set ID 0 reservation.
    const unsigned cell_res_id =
        reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id.value();
    const auto& res = cell_resources.dedicated[cell_res_id];
    parent->collision_manager.free(slot_alloc, res, rnti);
    reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id = std::nullopt;
  }

  // Clear parent pointer to avoid further reservations after commit.
  parent = nullptr;
}

void pucch_resource_manager::ue_reservation_guard::rollback()
{
  // If parent is nullptr, it means commit() was already called.
  if (parent != nullptr) {
    // Release all reservations made so far.
    for (auto& r : reservations) {
      if (r.cell_res_id.has_value()) {
        // Release the resource.
        const auto& res = cell_resources.dedicated[*r.cell_res_id];
        parent->collision_manager.free(slot_alloc, res, rnti);
      }
    }
  }
}
