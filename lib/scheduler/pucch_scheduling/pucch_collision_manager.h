// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"

namespace ocudu {

namespace detail {

/// \brief Collision matrix indicating which resources collide with each other.
///  - C[i][j] = 1 if resource i collides with resource j, 0 otherwise.
using collision_matrix = static_vector<bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>,
                                       pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>;

/// Compute the collision matrix for all PUCCH resources in the cell.
collision_matrix make_collision_matrix(const cell_pucch_res_config& cell_resources);

/// \brief Matrix of multiplexing regions indicating which resources can be multiplexed together.
///  - Each row represents a multiplexing region.
///  - M[i][j] = 1 if resource j belongs to multiplexing region i, 0 otherwise.
/// \remark Multiplexing regions with only one resource (i.e., non-multiplexed resources) are not represented in this
/// matrix. Therefore, we can have a maximum of $MAX_NOF_TOT_CELL_RESOURCES / 2$ rows.
using mux_regions_matrix = static_vector<bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>,
                                         pucch_constants::MAX_NOF_TOT_CELL_RESOURCES / 2>;

/// Compute the multiplexing matrix for all PUCCH resources in the cell.
mux_regions_matrix make_mux_regions_matrix(const cell_pucch_res_config& cell_resources);

} // namespace detail

/// Reasons for a PUCCH allocation failure.
enum class pucch_alloc_failure {
  /// The resource is already allocated to this UE in this slot.
  ALREADY_ALLOCATED,
  /// The resource is already allocated to another UE in this slot.
  RESOURCE_IN_USE,
  /// The resource collides with another PUCCH grant in this slot.
  PUCCH_COLLISION,
  /// The resource collides with another UL grant in this slot.
  UL_GRANT_COLLISION,
};

/// \brief This class manages PUCCH resource collisions within a cell.
///
/// It keeps track of the usage of both common and dedicated resources for each slot, and provides methods to allocate
/// and free resources while accounting for collisions with other resources or other UL grants. Each operation updates
/// the provided UL resource grid accordingly.
///
/// PUCCH-to-UL grant collisions are handled via the UL resource grid, while PUCCH-to-PUCCH collisions are handled by
/// precomputing which resources collide with each other, taking into account the multiplexing capabilities of PUCCH.
///
/// \remark To deal with the multiplexing capabilities of some PUCCH formats, we define the following concepts:
/// - Multiplexing index. A number that identifies the orthogonal sequence used by a PUCCH resource. Computed from:
///   - Format 0: initial cyclic shift.
///   - Format 1: initial cyclic shift and time domain OCC index.
///   - Format 2/3: not multiplexed (always 0).
///   - Format 4: OCC index.
/// - Multiplexing region. A set of PUCCH resources that overlap in time and frequency but can be allocated together:
///   - They share the same time-frequency allocation.
///   - They share the same PUCCH format.
///   - They have different multiplexing indices.
class pucch_collision_manager
{
public:
  using r_pucch_t = bounded_integer<unsigned, 0, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES - 1>;

  pucch_collision_manager(const cell_configuration& cell_cfg);

  void slot_indication(slot_point sl_tx);
  void stop();

  /// \brief Check if a PUCCH resource can be allocated at a given slot.
  /// \return Success if the resource can be allocated, otherwise an error indicating the reason of failure.
  error_type<pucch_alloc_failure>
  can_alloc(const cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti) const;

  /// \brief Allocate a PUCCH resource at a given slot.
  /// \return Success if the allocation was successful, otherwise an error indicating the reason of failure.
  error_type<pucch_alloc_failure>
  alloc(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti);

  /// Allocate a PUCCH resource at a given slot, without doing any checks.
  void do_alloc(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti);

  /// Free a common PUCCH resource at the given slot.
  /// \return True if the resource was successfully freed, false if the resource was not allocated to this UE.
  bool free(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti);

private:
  using mux_region_lookup_t = slotted_array<size_t, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>;

  const cell_configuration& cell_cfg;
  /// Precomputed collision matrix for all PUCCH resources.
  const detail::collision_matrix col_matrix;
  /// Precomputed multiplexing regions matrix for all PUCCH resources.
  const detail::mux_regions_matrix mux_matrix;
  /// Lookup table to get the multiplexing region of each resource.
  const mux_region_lookup_t mux_region_lookup;

  /// Allocation context for a specific slot.
  struct slot_context {
    /// Bitset representing the current usage state of all PUCCH resources (common and dedicated) in this slot.
    ///  - S[i] = 1 if resource i is in use, 0 otherwise.
    bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES> current_state;
    /// Map of currently allocated resources to their owners (RNTIs).
    std::vector<rnti_t> owners;
    /// Resource grid that keeps track of the time-frequency grants of PUCCH resources in this slot.
    cell_slot_resource_grid pucch_res_grid;

    /// Default constructor needed by circular_array.
    slot_context() : pucch_res_grid({}) {}

    /// Construct a slot context from the given cell configuration.
    slot_context(const cell_configuration& cell_cfg);

    /// Clear the slot context to the default state.
    void clear();
  };

  // Ring buffer of slot contexts to keep track of PUCCH resource usage in recent slots.
  circular_vector<slot_context> slots_ctx;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;

  /// Build the lookup table that maps each resource index to its multiplexing region.
  static mux_region_lookup_t build_mux_region_lookup(const detail::mux_regions_matrix& mux_matrix);
};

} // namespace ocudu
