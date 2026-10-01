// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
                                                        const ue_cell_configuration& ue_cell_cfg,
                                                        unsigned                     k0,
                                                        unsigned                     k1,
                                                        const pdcch_dl_information&  dci_info) override;

  std::optional<unsigned>
  alloc_ded_harq_ack(cell_resource_allocator&     res_alloc,
                     const ue_cell_configuration& ue_cell_cfg,
                     unsigned                     k0,
                     unsigned                     k1,
                     pucch_repetition_factor      max_rep_factor = pucch_repetition_factor::n1) override;

  bool alloc_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                            const ue_cell_configuration&  ue_cell_cfg) override;

  bool alloc_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                             const ue_cell_configuration&  ue_cell_cfg) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                          const ue_cell_configuration&  ue_cell_cfg) override;

  [[nodiscard]] bool has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const override;

  [[nodiscard]] span<const slot_point> get_pucch_repetition_slots(rnti_t rnti, slot_point sl_tx) const override;

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

  /// Maximum number of slots that a PUCCH HARQ-ACK transmission with repetitions can span.
  static constexpr unsigned max_nof_burst_slots = static_cast<unsigned>(pucch_repetition_factor::n8);

  /// \brief Slots spanned by a PUCCH HARQ-ACK repetition burst, in ascending order.
  ///
  /// A burst always spans at least 2 slots, as repetition factor \c n1 is a plain single-slot transmission and is never
  /// modelled as a burst. An empty list therefore means that no repetition is in use.
  using burst_slot_list = static_vector<slot_point, max_nof_burst_slots>;

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
    // If \c harq_ack is part of a multi-slot PUCCH repetition burst, all the slots (including this one) that make up
    // that burst, the first one being the burst's anchor slot; empty otherwise. Repetition PUCCH cannot be multiplexed
    // with SR or CSI, so no such grant may be added to a slot while this is set. Additional HARQ-ACK bits are still
    // allowed, and are propagated to every slot of the burst to keep the repeated payload in sync.
    burst_slot_list burst_slots;

    /// Returns whether \c harq_ack is part of a multi-slot PUCCH repetition burst.
    [[nodiscard]] bool harq_ack_is_repetition() const { return not burst_slots.empty(); }

    /// Returns the list of PUCCH PDU indices allocated to the UE, optionally including the common grant.
    [[nodiscard]] static_vector<stable_id_t, max_nof_ue_grants> pdu_indices(bool include_common = true) const;

    /// Returns the number of PUCCH grants allocated to the UE, optionally including the common grant.
    [[nodiscard]] unsigned nof_grants(bool include_common = true) const;

    /// Returns the total number of UCI bits allocated to the UE, based on the PUCCH PDUs currently allocated to the UE.
    [[nodiscard]] pucch_uci_bits uci_bits(const stable_id_map<pucch_info>& pdus) const;
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

  /// \brief Selects the d_pri to use for a given UE, based on the UCI bits to be sent and the PUCCH resources
  ///        available in the given slot.
  ///
  /// \remark Whether Resource Set ID 0 or Resource Set ID 1 is used is derived from \c bits.
  std::optional<unsigned> select_pri(const cell_slot_resource_allocator& pucch_slot_alloc,
                                     const ue_cell_configuration&        ue_cell_cfg,
                                     const pucch_uci_bits&               bits,
                                     const dci_context_information*      dci_info);

  // Commits to the collision manager only the dedicated (HARQ-ACK/SR/CSI) resources that changed with respect to
  // old_grants, before allocate_grants overwrites the old PDU entries in place.
  void commit_dedicated_grant_diff(cell_slot_resource_allocator& pucch_slot_alloc,
                                   const ue_grants&              old_grants,
                                   const pucch_grant_list&       new_grants,
                                   rnti_t                        rnti);

  // Implements the main steps of the multiplexing procedure as defined in TS 38.213, Section 9.2.5.
  // \c rep_state and \c rep_anchor_slot are only applied to the resulting HARQ-ACK grant, and are meant for PUCCH
  // repetition bursts.
  std::optional<ue_grants>
  multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                               const pucch_uci_bits&         new_bits,
                               const ue_grants&              old_grants,
                               const ue_cell_configuration&  ue_cell_cfg,
                               std::optional<unsigned>       d_pri,
                               const alloc_context&          alloc_ctx,
                               pucch_repetition_tx_slot      rep_state       = pucch_repetition_tx_slot::no_multi_slot,
                               slot_point                    rep_anchor_slot = {});

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

  /// Candidate for a PUCCH HARQ-ACK repetition burst.
  struct harq_ack_burst_candidate {
    /// PUCCH Resource Indicator of the candidate resource.
    unsigned pri;
    /// Slots the burst would span.
    burst_slot_list slots;
  };

  /// \brief Searches for a PUCCH HARQ-ACK repetition burst, i.e. a resource and the slots it would be repeated in.
  ///
  /// Repetition factors are static, per-PRI, cell-configured values. Starting from the largest factor not exceeding
  /// \c max_rep_factor, every resource of the resource set configured with that factor is tried, moving on to the next
  /// smaller factor until one of them can be used; \c n1 is not considered, as a single-slot transmission is not a
  /// repetition burst and is allocated through the regular path. Note that, since resources of the same resource set
  /// can have different symbols and formats, both the number of repetitions that fit and the slots where they land
  /// depend on the candidate resource (see \c find_burst_slots).
  ///
  /// \param[in] bits UCI bits the burst has to carry; they determine which Resource Set is searched.
  /// \param[in] anchor_delay Slot delay of the PUCCH occasion the burst must start at.
  /// \param[in] released_slots Slots whose PUCCH grants for this UE are released if the candidate gets committed, and
  /// which are therefore treated as free for this UE.
  std::optional<harq_ack_burst_candidate> find_harq_ack_burst_candidate(cell_resource_allocator&     res_alloc,
                                                                        const ue_cell_configuration& ue_cell_cfg,
                                                                        const pucch_uci_bits&        bits,
                                                                        pucch_repetition_factor      max_rep_factor,
                                                                        unsigned                     anchor_delay,
                                                                        span<const slot_point>       released_slots);

  /// \brief Determines the slots that a PUCCH transmission with \c nof_slots repetitions of a given resource, starting
  /// at the PUCCH occasion at \c anchor_delay, would be transmitted in.
  ///
  /// As per TS 38.213, Section 9.2.6, for unpaired spectrum the repetition slots are the slots whose UL symbols can
  /// host the resource; the UE skips over the slots that cannot and transmits the repetition later instead. The
  /// anchor slot, on the other hand, is dictated by the HARQ-ACK feedback timing and cannot be moved.
  ///
  /// \return The slots of the burst; \c std::nullopt if the resource cannot be used for all of them.
  std::optional<burst_slot_list> find_burst_slots(cell_resource_allocator& res_alloc,
                                                  const pucch_resource&    res,
                                                  unsigned                 nof_slots,
                                                  unsigned                 anchor_delay,
                                                  span<const slot_point>   released_slots,
                                                  rnti_t                   rnti);

  /// Allocates a HARQ-ACK-only PUCCH PDU carrying \c bits in every slot of a candidate burst.
  void commit_harq_ack_burst(cell_resource_allocator&        res_alloc,
                             const ue_cell_configuration&    ue_cell_cfg,
                             const harq_ack_burst_candidate& candidate,
                             const pucch_uci_bits&           bits,
                             const alloc_context&            alloc_ctx);

  /// Removes the PUCCH PDUs of an in-flight repetition burst and frees its resources.
  void release_harq_ack_burst(cell_resource_allocator& res_alloc, span<const slot_point> burst_slots, rnti_t rnti);

  /// \brief Attempts to allocate a multi-slot PUCCH HARQ-ACK repetition burst carrying \c bits, anchored at the PUCCH
  /// occasion at \c anchor_delay.
  ///
  /// Used both for the first HARQ-ACK bit of a UE (with an empty \c released_slots) and to re-select the burst of an
  /// in-flight one whose extra HARQ-ACK bit pushes it past the Resource Set 0 bit-count threshold (with the ongoing
  /// burst's slots as \c released_slots). Since resources can have different symbols and format across resource sets,
  /// neither the number of repetitions nor the slots they land on can be assumed to stay the same on a re-selection,
  /// so the burst is always searched from scratch. Nothing is mutated unless a candidate is found, so a failure leaves
  /// any ongoing burst, and the HARQ-ACK bits already allocated to it, untouched; the caller is then expected to fall
  /// back to a single-slot grant through the regular path.
  ///
  /// \return The d_pri of the committed burst; \c std::nullopt if no factor greater than \c n1 could be reserved.
  std::optional<unsigned> try_alloc_harq_ack_burst(cell_resource_allocator&     res_alloc,
                                                   const ue_cell_configuration& ue_cell_cfg,
                                                   const pucch_uci_bits&        bits,
                                                   unsigned                     anchor_delay,
                                                   pucch_repetition_factor      max_rep_factor,
                                                   span<const slot_point>       released_slots,
                                                   const alloc_context&         alloc_ctx);

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

  // \brief Ring of PUCCH allocations indexed by slot.
  circular_vector<slot_context, true> slots_ctx;

  const cell_configuration&                     cell_cfg;
  const unsigned                                max_pucch_grants_per_slot;
  const unsigned                                max_ul_grants_per_slot;
  const cell_pucch_res_config&                  cell_resources;
  const pucch_resource_builder_params&          res_params;
  const std::optional<csi_report_configuration> csi_report_cfg;
  slot_point                                    last_sl_ind;
  pucch_collision_manager                       col_manager;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu
