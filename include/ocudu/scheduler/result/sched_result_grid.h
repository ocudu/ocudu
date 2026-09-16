// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/stable_id_map.h"
#include "ocudu/scheduler/result/csi_rs_info.h"
#include "ocudu/scheduler/result/detail/sched_pdu_types.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/result/prach_info.h"
#include "ocudu/scheduler/result/prs_info.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/pusch_info.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/result/srs_info.h"
#include "ocudu/support/ocudu_assert.h"
#include <vector>

namespace ocudu {

/// Configuration of the dimensions of the scheduler result grid.
struct sched_result_grid_config {
  /// Number of slots held by the grid.
  unsigned nof_slots = 1;
  /// \brief [Implementation-defined] Number of slots that can hold PDUs at the same time.
  ///
  /// Sizes the pool of every pooled PDU list. A slot holds PDUs from the moment it is scheduled until it leaves the
  /// grid history, which spans fewer slots than the grid holds.
  unsigned nof_live_slots = 32;
};

namespace sched_result_context {

/// Broadcast allocations of a slot.
struct broadcast_slot {
  static_vector<ssb_information, MAX_SSB_PER_SLOT>     ssb_info;
  static_vector<sib_information, MAX_SI_PDUS_PER_SLOT> sibs;
};

/// Downlink storage of a slot in the scheduler result grid.
struct dl_slot {
  unsigned                                                      nof_dl_symbols = 0;
  pdcch_dl_info_list                                            dl_pdcchs;
  pdcch_ul_info_list                                            ul_pdcchs;
  broadcast_slot                                                bc;
  pooled_pdu_list<rar_information>                              rar_grants;
  static_vector<dl_paging_allocation, MAX_PAGING_PDUS_PER_SLOT> paging_grants;
  static_vector<dl_msg_alloc, MAX_UE_PDUS_PER_SLOT>             ue_grants;
  static_vector<csi_rs_info, MAX_CSI_RS_PDUS_PER_SLOT>          csi_rs;
  static_vector<prs_info, MAX_PRS_PDUS_PER_SLOT>                prs;
};

/// Uplink storage of a slot in the scheduler result grid.
struct ul_slot {
  unsigned                                                         nof_ul_symbols = 0;
  static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT>            puschs;
  static_vector<prach_occasion_info, MAX_PRACH_OCCASIONS_PER_SLOT> prachs;
  stable_id_map<pucch_info>                                        pucchs;
  static_vector<srs_info, MAX_SRS_PDUS_PER_SLOT>                   srss;
};

/// Storage of a slot in the scheduler result grid.
struct slot {
  bool                  success = false;
  failed_alloc_attempts failed_attempts;
  dl_slot               dl;
  ul_slot               ul;
};

/// Pools backing the pooled PDU lists of a cell.
class pdu_pool_set
{
  /// Selects the pool of a PDU type without an explicit specialization, which C++17 forbids at class scope.
  template <typename T>
  struct pdu_tag {};

public:
  explicit pdu_pool_set(const sched_result_grid_config& cfg) : rar_pool(MAX_RAR_PDUS_PER_SLOT * cfg.nof_live_slots) {}

  /// Fetches the pool that holds the PDUs of a given type.
  template <typename T>
  free_list_object_pool<T>& get()
  {
    return pool_of(pdu_tag<T>{});
  }

private:
  free_list_object_pool<rar_information>& pool_of(pdu_tag<rar_information> /* unused */) { return rar_pool; }

  free_list_object_pool<rar_information> rar_pool;
};

/// \brief Every PDU list a slot holds, in the order the slot declares them.
///
/// A list is reached by the type of PDU it holds, so no two entries may hold the same one.
using pdu_list_storages = type_list<decltype(dl_slot::dl_pdcchs),
                                    decltype(dl_slot::ul_pdcchs),
                                    decltype(broadcast_slot::ssb_info),
                                    decltype(broadcast_slot::sibs),
                                    decltype(dl_slot::rar_grants),
                                    decltype(dl_slot::paging_grants),
                                    decltype(dl_slot::ue_grants),
                                    decltype(dl_slot::csi_rs),
                                    decltype(dl_slot::prs),
                                    decltype(ul_slot::puschs),
                                    decltype(ul_slot::prachs),
                                    decltype(ul_slot::srss)>;

} // namespace sched_result_context

/// Builder of the list that holds the PDUs of a given type in a slot.
template <typename T>
using pdu_list_builder = typename sched_pdu_detail::get_list_builder<
    typename sched_pdu_detail::find_list_storage<T, sched_result_context::pdu_list_storages>::type>::type;

/// Read-only view over the list that holds the PDUs of a given type in a slot.
template <typename T>
using pdu_span = typename sched_pdu_detail::get_list_view<typename pdu_list_builder<T>::storage_type>::type;

/// Handle to read the downlink scheduler result of a slot.
class sched_slot_dl_result_reader
{
public:
  sched_slot_dl_result_reader() = default;
  sched_slot_dl_result_reader(const sched_result_context::dl_slot& storage_) : storage(&storage_) {}

  [[nodiscard]] bool valid() const { return storage != nullptr; }

  unsigned nof_dl_symbols() const { return storage->nof_dl_symbols; }

  pdu_span<pdcch_dl_information> dl_pdcchs() const { return pdu_span<pdcch_dl_information>{storage->dl_pdcchs}; }
  pdu_span<pdcch_ul_information> ul_pdcchs() const { return pdu_span<pdcch_ul_information>{storage->ul_pdcchs}; }
  pdu_span<ssb_information>      ssb_info() const { return pdu_span<ssb_information>{storage->bc.ssb_info}; }
  pdu_span<sib_information>      sibs() const { return pdu_span<sib_information>{storage->bc.sibs}; }
  pdu_span<rar_information>      rar_grants() const { return pdu_span<rar_information>{storage->rar_grants}; }
  pdu_span<dl_paging_allocation> paging_grants() const
  {
    return pdu_span<dl_paging_allocation>{storage->paging_grants};
  }
  pdu_span<dl_msg_alloc> ue_grants() const { return pdu_span<dl_msg_alloc>{storage->ue_grants}; }
  pdu_span<csi_rs_info>  csi_rs() const { return pdu_span<csi_rs_info>{storage->csi_rs}; }
  pdu_span<prs_info>     prs() const { return pdu_span<prs_info>{storage->prs}; }

private:
  const sched_result_context::dl_slot* storage = nullptr;
};

/// Handle to build the downlink scheduler result of a slot.
class sched_slot_dl_result_builder
{
public:
  sched_slot_dl_result_builder(sched_result_context::dl_slot& storage_, sched_result_context::pdu_pool_set& pools_) :
    storage(&storage_), pools(&pools_)
  {
  }

  pdu_list_builder<pdcch_dl_information> dl_pdcchs() { return {storage->dl_pdcchs, *pools}; }
  pdu_list_builder<pdcch_ul_information> ul_pdcchs() { return {storage->ul_pdcchs, *pools}; }
  pdu_list_builder<ssb_information>      ssb_info() { return {storage->bc.ssb_info, *pools}; }
  pdu_list_builder<sib_information>      sibs() { return {storage->bc.sibs, *pools}; }
  pdu_list_builder<rar_information>      rar_grants() { return {storage->rar_grants, *pools}; }
  pdu_list_builder<dl_paging_allocation> paging_grants() { return {storage->paging_grants, *pools}; }
  pdu_list_builder<dl_msg_alloc>         ue_grants() { return {storage->ue_grants, *pools}; }
  pdu_list_builder<csi_rs_info>          csi_rs() { return {storage->csi_rs, *pools}; }
  pdu_list_builder<prs_info>             prs() { return {storage->prs, *pools}; }

  /// Returns a reader over the PDUs allocated so far in the slot.
  sched_slot_dl_result_reader reader() const { return sched_slot_dl_result_reader{*storage}; }

private:
  sched_result_context::dl_slot*      storage;
  sched_result_context::pdu_pool_set* pools;
};

/// Handle to read the uplink scheduler result of a slot.
class sched_slot_ul_result_reader
{
public:
  sched_slot_ul_result_reader() = default;
  sched_slot_ul_result_reader(const sched_result_context::ul_slot& storage_) : storage(&storage_) {}

  [[nodiscard]] bool valid() const { return storage != nullptr; }

  unsigned nof_ul_symbols() const { return storage->nof_ul_symbols; }

  pdu_span<ul_sched_info>          puschs() const { return pdu_span<ul_sched_info>{storage->puschs}; }
  pdu_span<prach_occasion_info>    prachs() const { return pdu_span<prach_occasion_info>{storage->prachs}; }
  pdu_span<srs_info>               srss() const { return pdu_span<srs_info>{storage->srss}; }
  const stable_id_map<pucch_info>& pucchs() const { return storage->pucchs; }

private:
  const sched_result_context::ul_slot* storage = nullptr;
};

/// Handle to build the uplink scheduler result of a slot.
class sched_slot_ul_result_builder
{
public:
  sched_slot_ul_result_builder(sched_result_context::ul_slot& storage_, sched_result_context::pdu_pool_set& pools_) :
    storage(&storage_), pools(&pools_)
  {
  }

  pdu_list_builder<ul_sched_info>       puschs() { return {storage->puschs, *pools}; }
  pdu_list_builder<prach_occasion_info> prachs() { return {storage->prachs, *pools}; }
  pdu_list_builder<srs_info>            srss() { return {storage->srss, *pools}; }

  /// Fetches the PUCCH grants of the slot.
  stable_id_map<pucch_info>& pucchs() { return storage->pucchs; }

  /// Returns a reader over the PDUs allocated so far in the slot.
  sched_slot_ul_result_reader reader() const { return sched_slot_ul_result_reader{*storage}; }

private:
  sched_result_context::ul_slot*      storage;
  sched_result_context::pdu_pool_set* pools;
};

/// Handle to read the scheduler result of a slot.
class sched_slot_result_reader
{
public:
  sched_slot_result_reader() = default;
  sched_slot_result_reader(const sched_result_context::slot& storage_) : storage(&storage_) {}

  [[nodiscard]] bool valid() const { return storage != nullptr; }

  /// Whether the scheduling of the slot was successful.
  [[nodiscard]] bool success() const { return storage->success; }

  const failed_alloc_attempts& failed_attempts() const { return storage->failed_attempts; }

  sched_slot_dl_result_reader dl() const { return sched_slot_dl_result_reader{storage->dl}; }
  sched_slot_ul_result_reader ul() const { return sched_slot_ul_result_reader{storage->ul}; }

private:
  const sched_result_context::slot* storage = nullptr;
};

/// Handle to build the scheduler result of a slot.
class sched_slot_result_builder
{
public:
  sched_slot_result_builder(sched_result_context::slot& storage_, sched_result_context::pdu_pool_set& pools_) :
    storage(&storage_), pools(&pools_)
  {
  }

  void set_success(bool success) { storage->success = success; }

  failed_alloc_attempts& failed_attempts() { return storage->failed_attempts; }

  sched_slot_dl_result_builder dl() { return sched_slot_dl_result_builder{storage->dl, *pools}; }
  sched_slot_ul_result_builder ul() { return sched_slot_ul_result_builder{storage->ul, *pools}; }

  /// Returns a reader over the result built so far.
  sched_slot_result_reader reader() const { return sched_slot_result_reader{*storage}; }

private:
  sched_result_context::slot*         storage;
  sched_result_context::pdu_pool_set* pools;
};

/// \brief Grid of scheduler results, holding the decision of both DL and UL for each of its slots.
///
/// Slots are accessed through builder and reader handles, which are the only way to reach the stored PDUs.
class sched_result_grid
{
public:
  explicit sched_result_grid(const sched_result_grid_config& cfg) : pools(cfg), slots(cfg.nof_slots)
  {
    for (auto& sl : slots) {
      sl.ul.pucchs.reserve(MAX_PUCCH_PDUS_PER_SLOT);
      sl.dl.rar_grants.reserve(MAX_RAR_PDUS_PER_SLOT);
    }
  }

  /// Number of slots held by the grid.
  [[nodiscard]] size_t nof_slots() const { return slots.size(); }

  /// Fetches the builder of the result of a slot.
  sched_slot_result_builder get_builder(unsigned slot_idx)
  {
    ocudu_assert(slot_idx < slots.size(), "Slot index out of bounds");
    return sched_slot_result_builder{slots[slot_idx], pools};
  }

  /// Fetches a reader of the result of a slot.
  sched_slot_result_reader get_reader(unsigned slot_idx) const
  {
    ocudu_assert(slot_idx < slots.size(), "Slot index out of bounds");
    return sched_slot_result_reader{slots[slot_idx]};
  }

  /// \brief Discards the result of a slot and sets the symbols active in its new slot.
  ///
  /// The capacity of every PDU list is preserved, and the PDUs of the pooled lists return to their pool.
  void reset_slot(unsigned slot_idx, unsigned nof_dl_symbols, unsigned nof_ul_symbols)
  {
    ocudu_assert(slot_idx < slots.size(), "Slot index out of bounds");
    sched_result_context::slot& sl = slots[slot_idx];

    sl.success         = false;
    sl.failed_attempts = {};

    sl.dl.dl_pdcchs.clear();
    sl.dl.ul_pdcchs.clear();
    sl.dl.bc.ssb_info.clear();
    sl.dl.bc.sibs.clear();
    sl.dl.rar_grants.clear();
    sl.dl.paging_grants.clear();
    sl.dl.ue_grants.clear();
    sl.dl.csi_rs.clear();
    sl.dl.prs.clear();
    sl.dl.nof_dl_symbols = nof_dl_symbols;

    sl.ul.puschs.clear();
    sl.ul.prachs.clear();
    sl.ul.pucchs.clear();
    sl.ul.srss.clear();
    sl.ul.nof_ul_symbols = nof_ul_symbols;
  }

private:
  // Declared before the slots, so that the pools outlive the handles the slots hold into them.
  sched_result_context::pdu_pool_set      pools;
  std::vector<sched_result_context::slot> slots;
};

} // namespace ocudu
