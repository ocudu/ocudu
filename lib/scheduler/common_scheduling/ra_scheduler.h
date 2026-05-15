// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/cell_harq_manager.h"
#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../support/prbs_calculator.h"
#include "ocudu/adt/circular_map.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"

namespace ocudu {

class scheduler_event_logger;
class cell_metrics_handler;
struct ul_crc_indication;

/// Scheduler for RAR PDSCHs and Msg3 PUSCH grants and handler of RACH indications.
class ra_scheduler
{
public:
  explicit ra_scheduler(const cell_configuration& cfg_,
                        pdcch_resource_allocator& pdcch_sched_,
                        scheduler_event_logger&   ev_logger_,
                        cell_metrics_handler&     metrics_handler_);
  ~ra_scheduler();

  /// Enqueue RACH indication coming from lower layers.
  /// \note Potentially called from a different executor than the cell scheduler executor.
  void handle_rach_indication(const rach_indication_message& msg);

  /// Handle UL CRC ACKing/NACKing a Msg3 HARQ process.
  /// \note Potentially called from a different executor than the cell scheduler executor.
  void handle_crc_indication(const ul_crc_indication& crc_ind);

  /// Save an upcoming CFRA UE Ids.
  void handle_cfra_mapping_update(du_ue_index_t ue_index, rnti_t crnti);

  /// Allocate pending RARs + Msg3s
  void run_slot(cell_resource_allocator& res_alloc);

  /// Halt any pending allocations and stop RA scheduler activity.
  void stop();

private:
  class msg3_harq_timeout_notifier;
  class msgb_harq_timeout_notifier;
  class cached_bwp_info;

  struct pending_rar_failed_attempts_t {
    unsigned pdcch = 0;
    unsigned pdsch = 0;
    unsigned pusch = 0;
  };
  /// RAR grant pending to be scheduled.
  struct pending_rar_alloc {
    /// RA-RNTI generated for a given group of detected RACH preambles.
    rnti_t ra_rnti = rnti_t::INVALID_RNTI;
    /// Slot at which PRACH preambles were detected.
    slot_point prach_slot_rx;
    /// Last slot at which the scheduler attempted to allocated this RAR grant.
    slot_point last_sched_try_slot;
    /// Range of slots valid for RAR transmission.
    slot_interval rar_window;
    /// List of generated TC-RNTIs for each of the detected PRACH preambles.
    static_vector<rnti_t, MAX_PREAMBLES_PER_PRACH_OCCASION> tc_rntis;
    /// Attempts at scheduling a RAR and associated Msg3 grants.
    pending_rar_failed_attempts_t failed_attempts;
  };

  /// State of Msg3 grant pending to be scheduled and/or positive ACKed.
  struct pending_msg3_alloc {
    /// Detected PRACH Preamble associated to this Msg3 being scheduled.
    rach_indication_message::preamble preamble{};
    /// UL HARQ entity used to allocate an UL HARQ process for Msg3.
    /// Note: [TS 38.321, 5.4.2.1] "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is
    /// used".
    unique_ue_harq_entity harq_ent;
  };
  struct msg3_alloc_candidate {
    unsigned     pusch_td_res_index;
    crb_interval crbs;
  };

  /// State for a pending MsgB PDSCH (pending to be scheduled or waiting for a positive HARQ-ACK).
  struct pending_msgb_alloc {
    /// Per-preamble state carried into MsgB scheduling.
    struct preamble_ctx {
      rach_indication_message::preamble info;
      /// CRC outcome for the MsgA PUSCH.
      /// nullopt = indication not yet received; true = CRC OK (SuccessRAR); false = CRC KO (FallbackRAR).
      std::optional<bool> crc_result;
      /// Set to true once the MsgB grant for this preamble has been scheduled.
      bool msgb_scheduled = false;

      preamble_ctx(const rach_indication_message::preamble& info_) : info(info_) {}
    };

    rnti_t        msgb_rnti = rnti_t::INVALID_RNTI;
    slot_point    prach_slot_rx;
    slot_interval msgb_window;
    /// Last slot at which the scheduler attempted to allocate this MsgB grant.
    slot_point last_sched_try_slot;
    /// List of detected MsgA preambles multiplexed into this MsgB response.
    static_vector<preamble_ctx, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
    /// DL HARQ entity used for MsgB PDSCH retransmissions. Allocated when MsgB is first scheduled.
    unique_ue_harq_entity msgb_harq_ent;
  };

  /// Queue type used to store pending RACH indications.
  using rach_indication_queue = concurrent_queue<rach_indication_message, concurrent_queue_policy::lockfree_mpmc>;

  /// Queue type used to store pending CRC indications.
  using crc_indication_queue = concurrent_queue<ul_crc_indication, concurrent_queue_policy::lockfree_mpmc>;

  /// Pre-compute invariant fields of RAR PDUs (PDSCH, DCI, etc.) for faster scheduling.
  void precompute_rar_fields();

  /// Pre-compute invariant fields of Msg3 PDUs (PUSCH, DCI, etc.) for faster scheduling.
  void precompute_msg3_pdus();

  void handle_rach_indication_impl(const rach_indication_message& msg, cell_resource_allocator& res_alloc);

  /// Handle a PRACH occasion carrying Msg1 (4-step RACH) preambles.
  void handle_msg1_occasion(const rach_indication_message::occasion&      occ,
                            span<const rach_indication_message::preamble> preambles,
                            slot_point                                    prach_slot_rx);

  /// Handle a PRACH occasion carrying MsgA (2-step RACH) preambles and allocate their PUSCH receptions.
  void handle_msga_occasion(const rach_indication_message::occasion&      occ,
                            span<const rach_indication_message::preamble> preambles,
                            slot_point                                    prach_slot_rx,
                            cell_resource_allocator&                      res_alloc);

  void handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc);

  void log_postponed_rar(const pending_rar_alloc&  rar,
                         const char*               cause_str,
                         std::optional<slot_point> sl = std::nullopt) const;

  /// Delete RARs that are out of the RAR window.
  void update_pending_rars(slot_point pdcch_slot);

  /// Determines whether the resource grid for the provided slot has the conditions for RAR scheduling.
  bool is_slot_candidate_for_rar(const cell_slot_resource_allocator& slot_res_alloc);

  /// Schedule pending RARs in the cell resource grid.
  void schedule_pending_rars(cell_resource_allocator& res_alloc);

  /// Try scheduling pending RARs for the provided slot.
  void schedule_pending_rars(cell_resource_allocator& res_alloc, slot_point pdcch_slot);

  /// Find and allocate DL and UL resources for pending RAR and associated Msg3 grants.
  /// \return The number of allocated Msg3 grants.
  unsigned schedule_rar(pending_rar_alloc& rar, cell_resource_allocator& res_alloc, slot_point pdcch_slot);

  /// Schedule RAR grant and associated Msg3 grants in the provided scheduling resources.
  /// \param res_alloc Cell Resource Allocator.
  /// \param pending_rar pending RAR with an associated RA-RNTI that is going to be scheduled.
  /// \param pdcch_slot Slot where the PDCCH is going to be scheduled.
  /// \param rar_crbs CRBs of the RAR to be scheduled.
  /// \param pdsch_time_res_index Index of PDSCH time domain resource.
  /// \param msg3_candidates List of Msg3s with respective resource information (e.g. RBs and symbols) to allocate.
  void fill_rar_grant(cell_resource_allocator&         res_alloc,
                      const pending_rar_alloc&         pending_rar,
                      slot_point                       pdcch_slot,
                      crb_interval                     rar_crbs,
                      unsigned                         pdsch_time_res_index,
                      span<const msg3_alloc_candidate> msg3_candidates);

  /// Schedule retransmission of Msg3.
  void schedule_msg3_retx(cell_resource_allocator& res_alloc, pending_msg3_alloc& msg3_ctx) const;

  /// Schedule pending MsgB grants in the cell resource grid.
  void schedule_pending_msgbs(cell_resource_allocator& res_alloc);

  /// Try scheduling pending MsgBs for the provided slot.
  void schedule_pending_msgbs(cell_resource_allocator& res_alloc, slot_point pdcch_slot);

  sch_prbs_tbs get_nof_pdsch_prbs_required(unsigned time_res_idx, unsigned nof_ul_grants) const;

  /// Reserve space in the resource grid for the MsgA PUSCH so it is not taken by other UL grants.
  void reserve_msga_pusch_rbs(cell_resource_allocator& res_alloc);

  // Set the max number of slots the scheduler can look ahead in the resource grid (with respect to the current slot) to
  // find PDSCH space for RAR.
  static constexpr unsigned max_dl_slots_ahead_sched = 8U;

  // -- args.

  const scheduler_ra_expert_config& sched_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  scheduler_event_logger&           ev_logger;
  cell_metrics_handler&             metrics_hdlr;
  ocudulog::basic_logger&           logger = ocudulog::fetch_basic_logger("SCHED");

  // -- Derived from args.

  /// RA window size in number of slots.
  const unsigned           ra_win_nof_slots;
  const crb_interval       ra_crb_lims;
  const interval<unsigned> cfra_preambles;
  const bool               prach_format_is_long;
  /// Duration of a single PRACH occasion in slots.
  const unsigned prach_occasion_duration_slots;
  /// Bitmap of CRBs that might be used for PUCCH transmissions, to avoid scheduling MSG3-PUSCH over them.
  crb_bitmap pucch_crbs;

  /// Pre-cached information related to RAR for a given PDSCH time resource.
  struct rar_param_cached_data {
    dmrs_information dmrs_info;
    /// Number of PRBs and TBS required for different numbers of grants. Index=0 corresponds to 1 grant.
    std::vector<sch_prbs_tbs> prbs_tbs_per_nof_grants;
  };
  std::vector<rar_param_cached_data> rar_data;

  /// Modulation and coding scheme.
  sch_mcs_description rar_mcs_config;

  /// Pre-cached information related to Msg3 for different PUSCH time resources.
  struct msg3_param_cached_data {
    dci_ul_info       dci;
    pusch_information pusch;
  };
  std::vector<msg3_param_cached_data> msg3_data;
  sch_mcs_description                 msg3_mcs_config;

  /// Pre-cached information for the initial BWP.
  std::unique_ptr<cached_bwp_info> cached_init_bwp_info;

  // -- State.

  // Currently managed HARQ processes for Random Access in this cell (Msg3 UL retx + MsgB DL retx).
  cell_harq_manager ra_harqs;

  // RACH indications pending to be processed.
  rach_indication_queue pending_rachs;

  // CRC indications pending to be processed.
  crc_indication_queue pending_crcs;

  // List of pending RARs to be scheduled.
  std::vector<pending_rar_alloc> pending_rars;

  // Map of pending Msg3 grants to be scheduled or waiting for a positive HARQ-ACK.
  // Keyed by ring_idx = to_value(tc_rnti) % SIZE.
  circular_map<uint16_t, pending_msg3_alloc> pending_msg3s;

  // List of pending MsgBs (2-step RACH responses) to be scheduled.
  std::vector<pending_msgb_alloc> pending_msgbs;

  // Marks whether the next slot indication is the first.
  bool first_slot_flag = true;

  // Circular map of RNTIs associated with CFRA.
  std::vector<std::atomic<rnti_t>> pending_cfra_ues;
};

} // namespace ocudu
