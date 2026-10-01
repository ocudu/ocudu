// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_dl_harq_buffer_pool.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"

using namespace ocudu;

/// Derive maximum TB/MAC PDU length given a cell parameters.
static units::bytes derive_max_pdu_length(unsigned cell_nof_prbs, unsigned max_nof_layers)
{
  ocudu_assert(max_nof_layers >= 1, "Invalid number of layers");
  units::bits cw_max_size{pdsch_constants::MAX_NRE_PER_RB * cell_nof_prbs * max_nof_layers *
                          pdsch_constants::MAX_MODULATION_ORDER};
  return cw_max_size.round_up_to_bytes();
}

cell_dl_harq_buffer_pool::cell_dl_harq_buffer_pool(unsigned cell_nof_prbs,
                                                   unsigned max_nof_layers,
                                                   unsigned max_harqs_per_cell,
                                                   unsigned max_nof_ue_contexts) :
  max_pdu_len(derive_max_pdu_length(cell_nof_prbs, max_nof_layers).value()),
  nof_buffers(max_harqs_per_cell),
  logger(ocudulog::fetch_basic_logger("MAC")),
  ue_buffer_list_pool(max_nof_ue_contexts),
  cell_buffers(MAX_NOF_DU_UES),
  pool(std::make_unique<dl_harq_buffer_storage[]>(nof_buffers))
{
  // Preallocate all DL HARQ buffers and make them available in the free list, so that UEs do not need to allocate
  // buffers in their creation critical path.
  free_buffer_list.reserve(nof_buffers);
  for (unsigned i = 0; i != nof_buffers; ++i) {
    pool[i].buffer.resize(max_pdu_len);
    free_buffer_list.emplace_back(&pool[i]);
  }
}

void cell_dl_harq_buffer_pool::clear()
{
  for (unsigned i = 0; i != cell_buffers.size(); ++i) {
    if (cell_buffers[i] != nullptr) {
      deallocate_ue_buffers(to_du_ue_index(i));
    }
  }
}

bool cell_dl_harq_buffer_pool::allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs)
{
  ocudu_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
  ocudu_assert(nof_harqs <= MAX_NOF_HARQS, "Invalid maximum number of HARQs");

  if (cell_buffers[ue_index] != nullptr) {
    logger.error("ue={}: DL HARQ buffers already allocated for UE with matching ID", ue_index);
    return false;
  }

  // Note: The list is only handed over to the UE once all its buffers are allocated, so that a failure leaves no
  // resources reserved.
  auto ue_harqs = ue_buffer_list_pool.get();
  if (ue_harqs == nullptr) {
    logger.error("ue={}: No DL HARQ buffer lists available for new UE", ue_index);
    return false;
  }

  // Grow the list of HARQ buffers associated with this UE by reusing buffers from the pre-allocated free list.
  while (ue_harqs->size() < nof_harqs) {
    auto* buffer = allocate_buffer();
    if (buffer == nullptr) {
      logger.warning("ue={}: No DL HARQ buffers available for new UE", ue_index);
      for (auto* allocated_buffer : *ue_harqs) {
        free_buffer_list.emplace_back(allocated_buffer);
      }
      return false;
    }
    ue_harqs->emplace_back(buffer);
  }

  cell_buffers[ue_index] = std::move(ue_harqs);
  return true;
}

void cell_dl_harq_buffer_pool::deallocate_ue_buffers(du_ue_index_t ue_idx)
{
  ocudu_assert(is_du_ue_index_valid(ue_idx), "Invalid UE index");
  if (cell_buffers[ue_idx] == nullptr) {
    return;
  }

  // Move allocated HARQs for this UE into the free list.
  for (auto* harq_buffer : *cell_buffers[ue_idx]) {
    free_buffer_list.emplace_back(harq_buffer);
  }
  cell_buffers[ue_idx].reset();
}

cell_dl_harq_buffer_pool::dl_harq_buffer_storage* cell_dl_harq_buffer_pool::allocate_buffer()
{
  // Some buffers may be still in flight after user removal.
  auto it = std::find_if(free_buffer_list.rbegin(), free_buffer_list.rend(), [](const dl_harq_buffer_storage* buffer) {
    return buffer->ref_cnt.load(std::memory_order_relaxed) == 0;
  });
  if (it == free_buffer_list.rend()) {
    return nullptr;
  }

  auto* tmp = *it;
  free_buffer_list.erase(std::next(it).base());
  return tmp;
}
