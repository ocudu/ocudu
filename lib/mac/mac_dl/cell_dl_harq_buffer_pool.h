// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/support/memory_pool/free_list_memory_pool.h"
#include "ocudu/support/shared_transport_block.h"

namespace ocudu {

/// Handle to a DL HARQ buffer managed inside the pool that allows moving the ownership of the buffer to a shared
/// transport block.
class dl_harq_buffer_handle
{
  span<uint8_t>          buffer;
  std::atomic<unsigned>* ref_cnt;

public:
  dl_harq_buffer_handle(span<uint8_t> buffer_, std::atomic<unsigned>* ref_cnt_) : buffer(buffer_), ref_cnt(ref_cnt_)
  {
    ocudu_assert(ref_cnt, "Invalid reference counter");
    ref_cnt->fetch_add(1, std::memory_order::memory_order_relaxed);
  }

  ~dl_harq_buffer_handle()
  {
    if (ref_cnt) {
      ref_cnt->fetch_sub(1, std::memory_order::memory_order_acq_rel);
    }
  }

  dl_harq_buffer_handle(const dl_harq_buffer_handle& other)            = delete;
  dl_harq_buffer_handle& operator=(const dl_harq_buffer_handle& other) = delete;

  dl_harq_buffer_handle(dl_harq_buffer_handle&& other) noexcept :
    buffer(std::exchange(other.buffer, {})), ref_cnt(std::exchange(other.ref_cnt, nullptr))
  {
  }
  dl_harq_buffer_handle& operator=(dl_harq_buffer_handle&& other) = delete;

  /// Returns the buffer.
  span<uint8_t>       get_buffer() { return buffer; }
  span<const uint8_t> get_buffer() const { return buffer; }

  /// \brief Transfers the ownership of the underlying buffer to a \c shared_transport_block.
  /// \note This method can be only called once. An assertion is triggered is the buffer is already transferred.
  shared_transport_block transfer_to_buffer_view(size_t nof_bytes)
  {
    ocudu_assert(ref_cnt, "Cannot transfer an empty buffer");

    auto out_buffer = buffer.first(nof_bytes);
    buffer          = {};
    // As we transfer ownership, do not increment the reference counter inside the view.
    return shared_transport_block(out_buffer, std::exchange(ref_cnt, nullptr), false);
  }
};

/// Class that manages the allocation, deallocation and fetching of DL HARQ buffers for a given cell.
class cell_dl_harq_buffer_pool
{
  /// Type representing a DL HARQ buffer.
  struct dl_harq_buffer_storage {
    dl_harq_buffer_storage() = default;
    explicit dl_harq_buffer_storage(size_t initial_size) : buffer(initial_size) {}
    explicit dl_harq_buffer_storage(std::vector<uint8_t>&& buffer_) : buffer(std::move(buffer_)) {}

    /// Reference counter, values greater than 0 mean the buffer is being used, otherwise it is free to be reused.
    std::atomic<unsigned> ref_cnt{0};
    /// HARQ buffer storage.
    std::vector<uint8_t> buffer;
  };

  /// Type representing the list of DL HARQ buffers allocated for a given UE in a given cell.
  using ue_dl_harq_buffer_list = static_vector<dl_harq_buffer_storage*, MAX_NOF_HARQS>;

public:
  /// \brief Construction of a DL HARQ buffer pool for a given cell.
  ///
  /// \param cell_nof_prbs Number of PRBs of the cell.
  /// \param max_nof_layers Maximum number of DL layers configured for the cell.
  /// \param max_harqs_per_cell Maximum number of HARQs per cell.
  /// \param max_nof_ue_contexts Number of UE contexts that the cell is expected to hold.
  cell_dl_harq_buffer_pool(unsigned cell_nof_prbs,
                           unsigned max_nof_layers,
                           unsigned max_harqs_per_cell,
                           unsigned max_nof_ue_contexts);

  /// Called on cell deactivation to clear all available buffers.
  void clear();

  /// \brief Allocate DL HARQ buffers for a newly created UE.
  /// \return Whether the buffers were allocated.
  bool allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs);

  /// Deallocate DL HARQ buffers for a removed UE.
  void deallocate_ue_buffers(du_ue_index_t ue_index);

  /// Allocates a DL HARQ buffer for a given UE.
  expected<dl_harq_buffer_handle> allocate_dl_harq_buffer(du_ue_index_t ue_index, harq_id_t h_id)
  {
    ocudu_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
    ocudu_assert(cell_buffers[ue_index] != nullptr, "UE has no DL HARQ buffers allocated");
    ocudu_assert(cell_buffers[ue_index]->size() > h_id, "Invalid HARQ ID={}", h_id);

    auto* harq_buffer = (*cell_buffers[ue_index])[h_id];
    if (harq_buffer->ref_cnt.load(std::memory_order_acquire) != 0) {
      return make_unexpected(default_error_t{});
    }

    return dl_harq_buffer_handle(harq_buffer->buffer, &harq_buffer->ref_cnt);
  }

private:
  /// Takes an unused element from the free list (reference count is 0) and returns its pointer, otherwise returns
  /// nullptr if none is found.
  dl_harq_buffer_storage* allocate_buffer();

  /// Maximum MAC PDU length, derived based on the cell properties.
  const unsigned max_pdu_len;
  /// Number of DL HARQ buffers held by the pool.
  const unsigned nof_buffers;
  /// Logger.
  ocudulog::basic_logger& logger;
  // Note: Declared before the list of UEs, as the cell holds objects taken from it.
  /// Pool of DL HARQ buffer lists of the cell.
  free_list_object_pool<ue_dl_harq_buffer_list> ue_buffer_list_pool;
  /// DL HARQ buffers currently allocated to each UE of the cell, indexed by DU UE index.
  std::vector<free_list_object_pool<ue_dl_harq_buffer_list>::ptr> cell_buffers;
  /// DL HARQ buffers that are not associated with any UE and can be allocated to newly created UEs.
  std::vector<dl_harq_buffer_storage*> free_buffer_list;
  /// Stores the DL HARQ buffers.
  std::unique_ptr<dl_harq_buffer_storage[]> pool;
};

} // namespace ocudu
