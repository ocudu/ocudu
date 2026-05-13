// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief In this file, we provide a list of common checks to verify the validity of the scheduler output for a given
/// cell configuration. To maximize their applicability, these chosen checks should solely depend on the (assumed
/// immutable) cell configuration, and shouldn't depend on the scheduler or UE states. We assume that unit tests
/// checks should cover the latter cases.

#pragma once

#include "lib/scheduler/cell/resource_grid.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include <deque>
#include <unordered_map>

namespace ocudu {

class cell_configuration;
struct cell_resource_allocator;

/// \brief Current checks:
/// - CSI-RS, SSBs, PDCCHs and PDSCHs are only allocated in DL slots.
/// - PUCCHs, PUSCHs and PRACHs are only allocated in UL slots.
/// - In partial DL/UL slots, verifies that allocations are within the respective DL/UL symbols.
void assert_tdd_pattern_consistency(const cell_configuration& cell_cfg, slot_point sl_tx, const sched_result& result);

/// \brief Current checks:
/// - RNTIs match.
/// - The PDSCH symbols match chosen DCI time domain resource assignment.
/// - The PDSCH PRBs match the chosen DCI freq domain resource assignment.
void assert_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                           const pdcch_dl_information& pdcch,
                                           const pdsch_information&    pdsch);

/// \brief Verifies that DL PDCCHs have an associated PDSCH and that the common parameters between both match. Current
/// checks:
/// - A DL PDCCH always has an associated PDSCH with the same RNTI, taking into account k0 >= 0.
/// - The symbol and PRB parameters of the DCI content match the PDSCH parameters.
void assert_pdcch_pdsch_common_consistency(const cell_configuration&      cell_cfg,
                                           const cell_resource_allocator& cell_res_grid);

/// \brief Current checks:
/// - If CORESET#0 is not defined, SIB1 cannot be scheduled.
/// - SIB must use alloc_type_1.
/// - SIB PRBs must fall within CORESET#0 RB boundaries.
/// \param cell_cfg Cell configuration.
/// \param sibs Scheduled SIBs in a given slot.
void test_pdsch_sib_consistency(const cell_configuration& cell_cfg, span<const sib_information> sibs);

/// \brief Verify RAR content is valid. Current checks:
/// - RAR PRBS must fall within CORESET#0 RB boundaries.
/// - RAR must be alloc_type_1.
/// - No repeated RA-RNTIs across RAR grants and no repeated C-RNTIs across Msg3 grants.
/// - Consistent content in DCI of RARs (e.g. has to be f1_0, PRBs fall within CORESET#0 RB limits).
void test_pdsch_rar_consistency(const cell_configuration& cell_cfg, span<const rar_information> rars);

/// \brief Verify UE PDSCH content is valid. Current checks:
/// - PRBs fall within BWP boundaries.
void test_pdsch_ue_consistency(const cell_configuration& cell_cfg, span<const dl_msg_alloc> ue_grants);

/// \brief Verify UE PUSCH content is valid. Current checks:
/// - Number of PUSCHs does not exceed max_puschs_per_slot.
/// - PRBs fall within BWP boundaries.
void test_pusch_ue_consistency(const cell_configuration& cell_cfg, span<const ul_sched_info> ue_grants);

/// \brief Verify UE PUCCH content is valid. Current checks:
/// - Number of PUCCHs does not exceed max_pucchs_per_slot.
/// - Detects collisions between PUCCH grants.
void test_pucch_consistency(const cell_configuration& cell_cfg, span<const pucch_info> pucchs);

/// \brief Current checks:
/// - If sl_tx is not a valid PRACH SFN (n_SFN mod x != y), asserts no PRACH was allocated.
/// - PRACH occasions parameters match RACH-ConfigCommon present in cell_cfg.
/// \param cell_cfg Cell configuration.
/// \param sl_tx Slot being validated.
/// \param prachs Scheduled PRACH opportunities in a given slot.
void test_prach_opportunity_validity(const cell_configuration&       cell_cfg,
                                     slot_point                      sl_tx,
                                     span<const prach_occasion_info> prachs);

/// \brief Detects collisions in the "RB x symbol" DL resource grid.
void test_dl_resource_grid_collisions(const cell_configuration& cell_cfg, const dl_sched_result& result);

/// \brief Detects collisions in the "RB x symbol" UL resource grid.
void test_ul_resource_grid_collisions(const cell_configuration& cell_cfg, const ul_sched_result& result);

/// \brief Detects inconsistencies in the whole UL scheduling result. This function calls other more specific UL
/// validity checks.
void test_ul_consistency(const cell_configuration& cell_cfg, slot_point sl_tx, const ul_sched_result& result);

/// \brief Detects inconsistencies in the whole DL scheduling result. This function calls other more specific DL
/// validity checks.
void test_dl_consistency(const cell_configuration& cell_cfg, slot_point sl_tx, const dl_sched_result& result);

/// \brief Run all consistency checks for the scheduler result on a given slot.
void test_scheduler_result_consistency(const cell_configuration& cell_cfg,
                                       slot_point                sl_tx,
                                       const sched_result&       result);

/// \brief Run all consistency checks for multiple slot scheduler results.
void test_scheduler_result_consistency(const cell_configuration&      cell_cfg,
                                       const cell_resource_allocator& cell_res_grid);

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PDSCHs.
void assert_dl_resource_grid_filled(const cell_configuration& cell_cfg, const cell_resource_allocator& cell_res_grid);

struct ul_crc_indication;

namespace test_helper {

/// Helper test class to track the allocations of different RACH preambles.
class ra_scheduler_tracker
{
public:
  ra_scheduler_tracker(const cell_configuration& cell_cfg_);

  void on_new_rach_ind(const rach_indication_message& ind);
  void on_crc_indication(const ul_crc_indication& crc);

  void on_new_result(slot_point sl_tx, const sched_result& result);

  unsigned nof_ra_dl_pdcchs() const { return ra_dl_pdcch_ack_counter; }
  unsigned nof_rars() const { return rar_counter; }
  unsigned nof_msg3_newtxs() const { return msg3_newtx_counter; }
  unsigned nof_msg3_retxs() const { return msg3_retx_counter; }
  unsigned nof_msg3_acked() const { return msg3_ack_counter; }
  unsigned nof_msga_puschs() const { return msga_pusch_counter; }
  unsigned nof_success_rars() const { return success_rar_counter; }
  unsigned nof_fallback_rars() const { return fallback_rar_counter; }

  bool has_pending_ra() const
  {
    return not pending_rars.empty() or not pending_preambles.empty() or not pending_msg3_retxs.empty() or
           not pending_msga_preambles.empty();
  }

private:
  struct rar_context {
    pdcch_dl_information pdcch;
    slot_point           pdcch_slot;
    slot_point           rar_slot;
    bool                 scheduled = false;
  };
  struct preamble_context {
    rnti_t                            ra_rnti;
    rach_indication_message::preamble preamble;
    slot_interval                     rar_win;
    slot_point                        rar_slot;
    slot_point                        first_msg3_slot;
    slot_point                        last_msg3_slot;
    rar_ul_grant                      first_grant;
    bool                              acked = false;
  };
  struct msg3_retx_context {
    pdcch_ul_information pdcch;
    slot_point           pdcch_slot;
    slot_point           pusch_slot;
  };
  /// Tracks a 2-step RACH preamble from PRACH detection through MsgA PUSCH and MsgB scheduling.
  struct msga_preamble_context {
    rnti_t msgb_rnti;
    rnti_t tc_rnti;
    /// Expected MsgA PUSCH slot (prach_slot + td_offset).
    slot_point pusch_slot;
    /// End of the MsgB response window.
    slot_point msgb_window_stop;
    /// Whether the MsgA PUSCH was scheduled.
    bool pusch_sched = false;
    bool msgb_sched  = false;
  };

  bool is_expired(const preamble_context& ctxt, slot_point sl_tx) const;
  bool is_msga_preamble(uint8_t preamble_id) const;

  const cell_configuration& cell_cfg;
  unsigned                  prach_duration_slots = 0;

  unsigned ra_dl_pdcch_ack_counter = 0;
  unsigned rar_counter             = 0;
  unsigned msg3_newtx_counter      = 0;
  unsigned msg3_retx_counter       = 0;
  unsigned msg3_ack_counter        = 0;
  unsigned msga_pusch_counter      = 0;
  unsigned success_rar_counter     = 0;
  unsigned fallback_rar_counter    = 0;

  std::deque<rar_context>           pending_rars;
  std::deque<preamble_context>      pending_preambles;
  std::deque<msg3_retx_context>     pending_msg3_retxs;
  std::deque<msga_preamble_context> pending_msga_preambles;
};

/// Helper test class to track HARQ allocations and verify parameter consistency across retransmissions.
class harq_tracker
{
public:
  void on_new_result(slot_point sl_tx, const sched_result& result);

private:
  struct harq_alloc {
    bool         ndi;
    units::bytes tbs;
  };

  struct harq_key {
    rnti_t    rnti;
    harq_id_t harq_id;
    bool      operator==(const harq_key& o) const { return rnti == o.rnti and harq_id == o.harq_id; }
  };

  struct harq_key_hash {
    size_t operator()(const harq_key& k) const
    {
      return std::hash<uint32_t>{}((static_cast<uint32_t>(to_value(k.rnti)) << 8) | static_cast<uint8_t>(k.harq_id));
    }
  };

  std::unordered_map<harq_key, harq_alloc, harq_key_hash> dl_harqs;
  std::unordered_map<harq_key, harq_alloc, harq_key_hash> ul_harqs;
};

} // namespace test_helper

} // namespace ocudu
