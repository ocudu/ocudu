// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/ue_configuration.h"
#include "pucch_collision_manager.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include <array>
#include <optional>

namespace ocudu {

/// Container used to return the result of a PUCCH HARQ resource allocation request.
struct pucch_harq_resource_alloc_record {
  /// Pointer to PUCCH resource configuration to be used.
  const pucch_resource* resource;
  /// PUCCH resource indicator of the resource to be used.
  uint8_t pucch_res_indicator;
};

/// \brief Class that manages the cell allocation of PUCCH resources across UEs.
/// The correct functioning of pucch_resource_manager is based on the following assumptions:
/// (i)   Each UE has max 8 PUCCH F0/F1 and max 8 PUCCH F2/F3/F4 dedicated to HARQ-ACK reporting.
/// (ii)  Each UE has max 1 SR-dedicated PUCCH F0/F1 resource and max 1 CSI-dedicated PUCCH F2/F3/F4 resource.
/// (iii) The cell PUCCH resource list can have max \c MAX_NOF_CELL_PUCCH_RESOURCES PUCCH resource, including all
///       formats; at cell level, there is no constraint on how many resource must be F0/F1, F2/F3/F4, or for SR or for
///       CSI.
/// (iv)  UEs can have different PUCCH resource lists; however the PUCCH resource ID is unique within the cell. This
///       implies that if two UEs have the same PUCCH resource within their lists, their PUCCH resource ID must be the
///       same.
class pucch_resource_manager
{
public:
  pucch_resource_manager(const cell_configuration& cell_cfg_);

  void slot_indication(slot_point slot_tx);

  void stop();

  /// \brief Reserve the common PUCCH resource indexed by r_pucch, if available.
  bool reserve_harq_common_resource(cell_slot_resource_allocator& slot_alloc, size_t r_pucch, rnti_t rnti);

  /// \brief Release the common PUCCH resource indexed by r_pucch from being allocated to a given UE.
  void release_harq_common_resource(cell_slot_resource_allocator& slot_alloc, size_t r_pucch, rnti_t rnti);

  /// \brief RAII helper class that manages the reservation of dedicated PUCCH resources for a given UE at a given slot.
  /// The reservation is temporary until \c commit() is called, which makes the reservation permanent.
  /// \remark If \c commit() is not called before the destructor is invoked, all reservations are cancelled.
  class ue_reservation_guard
  {
  public:
    /// \brief RAII helper class that manages the reservation of dedicated PUCCH resources for a given UE.
    /// The reservation is temporary until \c commit() is called, which makes the reservation permanent.
    /// \remark If \c commit() is not called before the destructor is invoked, all reservations are cancelled.
    ue_reservation_guard(pucch_resource_manager*       parent_,
                         cell_slot_resource_allocator& slot_alloc_,
                         rnti_t                        rnti_,
                         const ue_cell_configuration&  ue_cfg_);

    ~ue_reservation_guard();

    // Disable copy and move semantics.
    ue_reservation_guard(const ue_reservation_guard&)                = delete;
    ue_reservation_guard& operator=(const ue_reservation_guard&)     = delete;
    ue_reservation_guard(ue_reservation_guard&&) noexcept            = delete;
    ue_reservation_guard& operator=(ue_reservation_guard&&) noexcept = delete;

    rnti_t     get_rnti() const { return rnti; }
    slot_point get_slot() const { return slot_alloc.slot; }

    /// \brief Reserve the next available PUCCH resource in PUCCH Resource Set ID 0.
    /// \return If there is any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the
    /// PUCCH resource indicator of the resource that will be used by the UE. Otherwise, the pointer will be \c nullptr,
    /// and \c d_pri is to be ignored.
    pucch_harq_resource_alloc_record reserve_harq_set_0_resource_next_available();

    /// \brief Reserve the next available PUCCH resource in PUCCH Resource Set ID 1.
    /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
    /// \return If there is any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the
    /// PUCCH resource indicator of the resource that will be used by the UE. Otherwise, the pointer will be \c nullptr,
    /// and \c d_pri is to be ignored.
    pucch_harq_resource_alloc_record reserve_harq_set_1_resource_next_available();

    /// \brief Reserve a specific PUCCH Resource Set ID 0 resource by its PUCCH resource indicator.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_harq_set_0_resource_by_res_indicator(unsigned d_pri);

    /// \brief Reserve a specific PUCCH Resource Set ID 1 resource by its PUCCH resource indicator.
    /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_harq_set_1_resource_by_res_indicator(unsigned d_pri);

    /// \brief Reserve the UE's SR PUCCH resource, if available.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_sr_resource();

    /// \brief Reserve the UE's CSI PUCCH resource, if available.
    /// \remark If SR multiplexing is enabled, this resource can be used for CSI + SR.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_csi_resource();

    /// Peek at the configured SR PUCCH resource for the UE without reserving it.
    const pucch_resource* peek_sr_resource() const;

    /// Peek at the configured CSI PUCCH resource for the UE without reserving it.
    const pucch_resource* peek_csi_resource() const;

    /// \brief Release PUCCH resource from Resource Set ID 0 from being allocated to a given UE.
    /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_harq_set_0_resource();

    /// \brief Release PUCCH resource from Resource Set ID 1 from being allocated to a given UE.
    /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_harq_set_1_resource();

    /// \brief Release SR PUCCH resource from being allocated to a given UE.
    /// \param[in] slot_sr slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_sr_resource();

    /// \brief Release CSI PUCCH resource from being allocated to a given UE.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_csi_resource();

    /// \brief Commit all the reservations made so far.
    void commit();

    /// \brief Rollback all the reservations made so far.
    void rollback();

  private:
    /// Defines the PUCCH resource usage.
    enum class resource_usage_type { harq_set_0, harq_set_1, sr, csi, nof_usage_types };

    struct reservation {
      std::optional<unsigned> cell_res_id;
    };
    pucch_resource_manager*              parent;
    cell_slot_resource_allocator&        slot_alloc;
    const pucch_resource_builder_params& res_params;
    const cell_pucch_res_config&         cell_resources;
    const rnti_t                         rnti;
    const ue_uplink_bwp_config&          ue_bwp_cfg;

    // Tracks the reservations made for the UE before commit().
    // Does not include reservations made prior to the creation of this guard.
    std::array<reservation, static_cast<size_t>(resource_usage_type::nof_usage_types)> reservations;

    // Helper functions that implement the public interface methods.
    template <unsigned ResourceSetId>
    pucch_harq_resource_alloc_record reserve_next_harq_res_available();

    template <unsigned ResourceSetId>
    const pucch_resource* reserve_harq_resource_by_res_indicator(unsigned d_pri);

    template <unsigned ResourceSetId>
    bool release_harq_resource();
  };

private:
  friend class ue_reservation_guard;

  const cell_configuration&    cell_cfg;
  const cell_pucch_res_config& cell_resources;
  pucch_collision_manager      collision_manager;
};

} // namespace ocudu
