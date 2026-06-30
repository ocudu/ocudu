// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_collision_manager.h"
#include "ocudu/adt/static_flat_map.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/scheduler/result/sched_result.h"

namespace ocudu {

/// Implementation of the PUCCH allocator interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_,
                                unsigned                  max_pucchs_per_slot,
                                unsigned                  max_ul_grants_per_slot_);

  ~pucch_allocator_impl() override;

  /// Updates the internal slot_point and tracking of PUCCH resource usage; and resets the PUCCH common allocation grid.
  void slot_indication(slot_point sl_tx) override;

  /// Called on cell deactivation.
  void stop();

  std::optional<unsigned> alloc_common_harq_ack(cell_resource_allocator&    res_alloc,
                                                rnti_t                      tcrnti,
                                                unsigned                    k0,
                                                unsigned                    k1,
                                                const pdcch_dl_information& dci_info) override;

  std::optional<unsigned> alloc_common_and_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                        rnti_t                       rnti,
                                                        const ue_cell_configuration& ue_cell_cfg,
                                                        unsigned                     k0,
                                                        unsigned                     k1,
                                                        const pdcch_dl_information&  dci_info) override;

  std::optional<unsigned> alloc_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                             rnti_t                       crnti,
                                             const ue_cell_configuration& ue_cell_cfg,
                                             unsigned                     k0,
                                             unsigned                     k1) override;

  bool alloc_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                            rnti_t                        crnti,
                            const ue_cell_configuration&  ue_cell_cfg) override;

  bool alloc_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                             rnti_t                        crnti,
                             const ue_cell_configuration&  ue_cell_cfg,
                             unsigned                      csi_part1_nof_bits) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        crnti,
                                          const ue_cell_configuration&  ue_cell_cfg) override;

  [[nodiscard]] bool has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const override;

private:
  /// ////////////  Helper struct and classes   //////////////

  /// \brief Defines the type of PUCCH resource.
  /// - harq_ack indicates the HAR-ACK resource (it can carry HARQ-ACK and/or SR and/or CSI bits).
  /// - sr indicates the resource dedicated for SR (it can carry SR and HARQ-ACK bits).
  /// - csi indicates the resource dedicated for CSI (it can carry CSI and SR bits).
  enum class pucch_resource_type { harq_ack, sr, csi };

  /// Converts a pucch_grant_type to string.
  static const char* to_string(pucch_resource_type type)
  {
    switch (type) {
      case pucch_resource_type::harq_ack:
        return "HARQ-ACK";
      case pucch_resource_type::sr:
        return "SR";
      case pucch_resource_type::csi:
        return "CSI";
      default:
        return "unknown";
    }
  }

  /// \brief Defines a PUCCH grant (and its relevant information) currently allocated to a given UE.
  /// It is used internally to keep track of the UEs' allocations of the PUCCH grants with dedicated resources.
  struct pucch_grant {
    pucch_resource_type   type;
    const pucch_resource* res = nullptr;
    pucch_uci_bits        bits;
  };

  /// \brief List of possible PUCCH grants that allocated to a UE, at a given slot.
  class pucch_grant_list
  {
  public:
    std::optional<pucch_grant> harq_ack;
    std::optional<pucch_grant> sr;
    std::optional<pucch_grant> csi;
    // Only relevant if there is a HARQ-ACK grant.
    unsigned d_pri = 0U;

    [[nodiscard]] unsigned nof_grants() const;
  };

  /// Keeps track of the PUCCH grants (both common and dedicated) for a given UE.
  struct ue_grants {
    /// [Implementation-defined] Corresponds to the case of the UE having common, F1 HARQ-ACK, and F1 SR grants.
    static constexpr unsigned max_nof_ue_grants = 3U;

    std::optional<stable_id_t> common;
    std::optional<stable_id_t> harq_ack;
    std::optional<stable_id_t> sr;
    std::optional<stable_id_t> csi;
    // Only relevant if there is a HARQ-ACK grant.
    unsigned d_pri = 0U;

    [[nodiscard]] static_vector<stable_id_t, max_nof_ue_grants> pdu_indices(bool include_common = true) const;
    [[nodiscard]] unsigned                                      nof_grants(bool include_common = true) const;
    [[nodiscard]] pucch_uci_bits                                uci_bits(const stable_id_map<pucch_info>& pdus) const;
  };

  /// Keeps track of the PUCCH allocation context for a given slot.
  struct slot_context {
    static_flat_map<rnti_t, ue_grants, MAX_PUCCH_PDUS_PER_SLOT> ue_grants_map;

    /// Clears the slot context.
    void clear() { ue_grants_map.clear(); }

    /// Finds the UE grants for a given RNTI.
    [[nodiscard]] ue_grants* find_ue_grants(rnti_t rnti)
    {
      auto it = ue_grants_map.find(rnti);
      return it != ue_grants_map.end() ? &it->second : nullptr;
    }
  };

  /// \brief Context information for a PUCCH allocation attempt.
  struct alloc_context;

  ///////////////  Main private functions   //////////////

  std::optional<unsigned> select_pri(const cell_slot_resource_allocator& pucch_slot_alloc,
                                     const ue_cell_configuration&        ue_cell_cfg,
                                     const pucch_uci_bits&               bits,
                                     const dci_context_information*      dci_info);

  // Implements the main steps of the multiplexing procedure as defined in TS 38.213, Section 9.2.5.
  std::optional<ue_grants> multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                        const pucch_uci_bits&         new_bits,
                                                        const ue_grants&              old_grants,
                                                        const ue_cell_configuration&  ue_cell_cfg,
                                                        std::optional<unsigned>       d_pri,
                                                        const alloc_context&          alloc_ctx);

  // Computes which resources are expected to be sent, depending on the UCI bits to be sent, before any multiplexing.
  static pucch_grant_list get_resources_pre_multiplexing(const ue_cell_configuration& ue_cell_cfg,
                                                         const pucch_uci_bits&        bits,
                                                         std::optional<unsigned>      d_pri);

  // Execute the multiplexing algorithm as defined in TS 38.213, Section 9.2.5.
  pucch_grant_list multiplex_resources(const ue_cell_configuration& ue_cell_cfg,
                                       const pucch_grant_list&      candidate_grants);

  // Applies the multiplexing rules depending on the PUCCH resource format, as per TS 38.213, Section 9.2.5.1/2.
  static std::optional<pucch_grant> merge_pucch_resources(const ue_cell_configuration& ue_cell_cfg,
                                                          span<const pucch_grant>      resources_to_merge,
                                                          unsigned                     d_pri);

  // Fast path for an additional HARQ-ACK bit that doesn't change the multiplexing outcome (see \c alloc_ded_harq_ack).
  std::optional<unsigned> update_harq_ack_bits(cell_slot_resource_allocator& pucch_slot_alloc,
                                               const ue_grants&              grants,
                                               unsigned                      harq_ack_nof_bits,
                                               const alloc_context&          alloc_ctx);

  // Allocate the PUCCH PDUs in the scheduler output, depending on the new PUCCH grants to be transmitted, and depending
  // on the PUCCH PDUs currently allocated.
  std::optional<ue_grants> allocate_grants(cell_slot_resource_allocator& pucch_slot_alloc,
                                           const ue_cell_configuration&  ue_cell_cfg,
                                           const ue_grants&              old_grants,
                                           const pucch_grant_list&       new_grants,
                                           const alloc_context&          alloc_ctx);

  ///////////////  Private helpers   ///////////////

  /// Returns whether a given UE can be allocated PUCCH in a given slot.
  bool can_allocate_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                          const ue_grants*                    existing_ue_grants,
                          const alloc_context&                alloc_ctx) const;

  /// Returns whether a given fallback UE can be allocated PUCCH in a given slot.
  bool can_allocate_fallback_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                                   const ue_grants*                    existing_ue_grants,
                                   const alloc_context&                alloc_ctx) const;

  /// Returns whether there is space for new PUCCH grants in the given scheduler result.
  bool is_there_space_for_new_pucch_grants(const sched_result& slot_result, unsigned nof_grants_to_allocate) const;

  /// Allocates the PUCCH resources for a given UE in the resource manager.
  void alloc_resources(cell_slot_resource_allocator& slot_alloc, const ue_grants& grants, rnti_t rnti);

  /// Frees the PUCCH resources for a given UE in the resource manager.
  void free_resources(cell_slot_resource_allocator& slot_alloc, const ue_grants& grants, rnti_t rnti);

  // \brief Ring of PUCCH allocations indexed by slot.
  circular_vector<slot_context> slots_ctx;

  const cell_configuration&                     cell_cfg;
  const unsigned                                max_pucch_grants_per_slot;
  const unsigned                                max_ul_grants_per_slot;
  const cell_pucch_res_config&                  cell_resources;
  const pucch_resource_builder_params&          res_params;
  const std::optional<csi_report_configuration> csi_report_cfg;
  slot_point                                    last_sl_ind;
  pucch_collision_manager                       resource_manager;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu
