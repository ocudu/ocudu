// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cell_dl_harq_buffer_pool.h"
#include "dl_sch_pdu_assembler.h"
#include "mac_cell_pcap_writer.h"
#include "mac_dl_metric_handler.h"
#include "mac_dl_ue_repository.h"
#include "mac_scheduler_cell_info_handler.h"
#include "mac_subframe_time_mapper_impl.h"
#include "paging_pdu_assembler.h"
#include "rar_pdu_assembler.h"
#include "sib_pdu_assembler.h"
#include "ssb_assembler.h"
#include "ocudu/mac/phy_cell_operation_controller.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/memory_pool/ring_buffer_pool.h"

namespace ocudu {

class timer_manager;

class mac_cell_processor final : public mac_cell_slot_handler, public mac_dl_cell_controller
{
public:
  mac_cell_processor(const mac_cell_creation_request& cell_cfg_req,
                     mac_scheduler_cell_info_handler& sched,
                     mac_slot_time_handler&           sfn_time_mapper_,
                     du_rnti_table&                   rnti_table,
                     mac_cell_result_notifier&        phy_notifier,
                     task_executor&                   cell_exec,
                     task_executor&                   slot_exec,
                     task_executor&                   ctrl_exec,
                     mac_pcap&                        pcap,
                     timer_manager&                   timers,
                     mac_cell_config_dependencies     dependencies);

  /// Starts configured cell.
  async_task<void> start() override;

  /// Stops configured cell.
  async_task<void> stop() override;

  void start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler,
                       const si_update_command&                      cmd,
                       std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier) override;

  void handle_si_update(const si_update_command& cmd) override;

  async_task<void> reconfigure(const mac_dl_cell_reconfig_request& request) override;

  void handle_slot_indication(const mac_cell_timing_context& context) noexcept override;
  void handle_error_indication(slot_point sl_tx, error_event event) noexcept override;
  void handle_stop_indication() noexcept override;

  /// Creates new UE DL context, updates logical channel MUX, adds UE in scheduler.
  async_task<bool> add_ue(const mac_ue_create_request& request);

  /// Deletes UE context in MAC MUX.
  async_task<void> remove_ue(const mac_ue_delete_request& request);

  /// Add/Modify UE bearers in the MUX.
  async_task<bool> addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_config> logical_channels);

  /// Remove UE bearers in the MUX.
  async_task<bool> remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem);

private:
  void handle_slot_indication_impl(slot_point_extended sl_tx, metric_clock::time_point enqueue_slot_tp) noexcept;

  /// Assemble struct that is going to be passed down to PHY with the DL scheduling result.
  /// \remark FAPI will use this struct to generate a DL_TTI.Request.
  void assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                 slot_point             sl_tx,
                                 du_cell_index_t        cell_index,
                                 const dl_sched_result& dl_res);

  /// Assemble struct with the MAC PDUs of a given slot and cell that is going be passed to the PHY.
  /// \remark FAPI will use this struct to generate a Tx_Data.Request.
  void assemble_dl_data_request(mac_dl_data_result&    mac_data,
                                slot_point_extended    sl_tx,
                                du_cell_index_t        cell_index,
                                const dl_sched_result& dl_res);

  /// Update DL buffer states of the allocated DL bearers.
  void update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res);

  // Dependencies.
  ocudulog::basic_logger&         logger;
  const mac_cell_creation_request cell_cfg;
  task_executor&                  cell_exec;
  task_executor&                  slot_exec;
  task_executor&                  ctrl_exec;
  mac_cell_result_notifier&       phy_cell;
  timer_manager&                  timers;

  // Mapper of upper-layer bearers to MAC logical channels in the DL direction.
  mac_dl_ue_repository ue_mng;

  // Pool of DL HARQ buffers used for UE PDSCH.
  cell_dl_harq_buffer_pool dl_harq_buffers;

  ticking_ring_buffer_pool pdu_pool;

  /// ssb_helper: contains the SSB-specific parameters that are derived from those passed by the DU interface. These
  /// parameters are passed to the scheduler and also to the PHY to generate the SSB PDU and PBCH payload.
  ssb_assembler ssb_helper;

  sib_pdu_assembler    sib_assembler;
  rar_pdu_assembler    rar_assembler;
  dl_sch_pdu_assembler dlsch_assembler;
  paging_pdu_assembler paging_assembler;

  mac_scheduler_cell_info_handler& sched;

  /// Ticks the APP clock based on the received slot indications for this cell.
  std::unique_ptr<mac_cell_clock_controller> time_source;

  // Handler of cell metrics
  mac_dl_cell_metric_handler metrics;

  // Represents cell activation state.
  enum class cell_state { inactive, active } state = cell_state::inactive;
  manual_event_flag stop_completed;

  /// \brief Optional PHY cell operation controller. When set, start()/stop() propagate to the PHY via FAPI P5
  /// START/STOP procedures. When null, MAC toggles its own state only and the PHY is not notified.
  phy_cell_operation_controller* phy_cell_op_controller = nullptr;

  /// True until start() has run once. The very first activation happens during DU.start() while
  /// FAPI executors are not yet pumping their queues, so awaiting the FAPI START handshake would
  /// deadlock. On the first call we skip the await and rely on the gate defaulting to active.
  /// Subsequent activations (runtime lock/unlock) take the full FAPI path.
  bool is_first_activation = true;

  mac_cell_pcap_writer pcap_writer;

  /// Reference to the subframe time mapper shared across all cells.
  mac_slot_time_handler& sfn_time_mapper;
};

} // namespace ocudu
