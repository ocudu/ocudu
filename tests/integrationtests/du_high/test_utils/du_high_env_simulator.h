// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high_sim_dependencies.h"
#include "du_high_ue_simulator.h"
#include "du_high_worker_manager.h"
#include "lib/gtpu/gtpu_teid_pool_impl.h"
#include "tests/test_doubles/f1u/dummy_f1u_du_gateway.h"
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/ran/srs/srs_properties.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/async/event_signal.h"
#include <list>

namespace ocudu {

class io_broker;
class mac_clock_controller;

namespace odu {

/// Parameters to set the DU-high environment simulator.
struct du_high_env_sim_params {
  unsigned                                     nof_cells  = 1;
  bool                                         auto_start = true;
  std::optional<cell_config_builder_params>    builder_params;
  std::optional<pucch_resource_builder_params> pucch_cfg;
  std::optional<scheduler_ue_expert_config>    sched_ue_expert_cfg;
  std::optional<unsigned>                      prach_frequency_start;
  std::optional<srs_periodicity>               srs_period;
  bool                                         active_cells_on_start = true;
  /// Whether a failed F1-C TNL connection is retried instead of closing the application.
  bool retry_f1c_connection = false;
};

du_high_configuration create_du_high_configuration(const du_high_env_sim_params& params = {});

class du_high_env_simulator
{
public:
  du_high_env_simulator(du_high_env_sim_params params = du_high_env_sim_params{});
  explicit du_high_env_simulator(const du_high_configuration& du_hi_cfg,
                                 bool                         auto_start            = true,
                                 bool                         active_cells_on_start = true);
  virtual ~du_high_env_simulator();

  bool add_ue(rnti_t rnti, du_cell_index_t cell_index = to_du_cell_index(0));

  /// Run the RRC setup procedure for the given RNTI from the moment the CU-CP sends an RRC Setup (via DL RRC Message
  /// Transfer) until the CU receives the RRC Setup Complete (via UL RRC Message Transfer).
  bool run_rrc_setup(rnti_t rnti);

  /// Run the RRC reject procedure for a given RNTI.
  bool run_rrc_reject(rnti_t rnti);

  /// Run the RRC Reestablishment procedure for the given RNTI from the moment the CU-CP sends an RRC Reestablishment
  /// (via DL RRC Message Transfer) until the CU receives the RRC Reestablishment Complete (via UL RRC Message
  /// Transfer).
  enum class reestablishment_stage { reest_complete, reconf_complete };
  [[nodiscard]] bool run_rrc_reestablishment(rnti_t                rnti,
                                             rnti_t                old_rnti,
                                             reestablishment_stage stop_at = reestablishment_stage::reconf_complete);

  bool run_ue_context_setup(rnti_t rnti);

  /// \brief Run the UE Context Modification procedure that reports the UE capabilities to the DU.
  ///
  /// The CU-CP runs the UE capability enquiry after the UE Context Setup, so the capabilities of a UE that attaches
  /// reach the DU in this procedure.
  ///
  /// \param[in] rnti            Identifier of the UE.
  /// \param[in] ue_capabilities Capabilities that the CU reports for the UE.
  bool run_ue_context_modification(rnti_t rnti, const byte_buffer& ue_capabilities);

  bool run_ue_context_release(rnti_t rnti, srb_id_t srb_id = srb_id_t::srb1);

  void run_slot();

  bool run_until(unique_function<bool()> condition, std::optional<unsigned> max_slot_count = std::nullopt);

  virtual void handle_slot_results(du_cell_index_t cell_index);

  /// \brief Builds the UCI that the simulated UEs report on the given PUSCH grants.
  ///
  /// By default every HARQ-ACK is an ACK and every CSI bit is set. Override it to report a specific CSI payload.
  virtual std::optional<mac_uci_indication_message> create_pusch_uci_indication(slot_point                sl_rx,
                                                                                span<const ul_sched_info> puschs);

  /// Schedule asynchronous task to be executed in the simulator context.
  void schedule_task(async_task<void> task);

  /// Launch non-blocking task to create a UE.
  async_task<void>
  launch_ue_creation_task(rnti_t rnti, du_cell_index_t cell_index = to_du_cell_index(0), bool assert_success = true);

  /// Launch non-blocking task to run RRC Setup procedure.
  async_task<void> launch_rrc_setup_task(rnti_t rnti, bool assert_success = true);

  /// Launch non-blocking task to run UE Context Release.
  async_task<void>
  launch_ue_context_release_task(rnti_t rnti, srb_id_t srb_id = srb_id_t::srb1, bool assert_success = true);

  /// Outcome of the random access procedures run by \ref launch_rach_attempts_task.
  struct rach_attempts_result {
    /// Number of RACH indications forwarded to the DU.
    unsigned nof_attempts = 0;
    /// Number of them whose RAR the scheduler sent within the RA window.
    unsigned nof_rars_received = 0;
  };

  /// \brief Launch non-blocking task to run \c nof_attempts contention-based random access procedures.
  ///
  /// The task detects one preamble in each of the next \c nof_attempts PRACH occasions of the cell and awaits the RAR
  /// of each of them.
  ///
  /// \param[out] result           Outcome of the attempts, filled in as the task progresses.
  /// \param prach_ind_delay_slots Number of slots the RACH indication takes to reach the DU, counted from the PRACH
  ///                              occasion it reports. It models the PHY preamble detection and the FAPI transport.
  async_task<void> launch_rach_attempts_task(unsigned              nof_attempts,
                                             rach_attempts_result& result,
                                             unsigned              prach_ind_delay_slots = 3,
                                             du_cell_index_t       cell_index            = to_du_cell_index(0));

  /// Await completion of all pending asynchronous tasks.
  void run_until_all_pending_tasks_completion();

  du_high_worker_manager                workers;
  timer_manager                         timers;
  std::unique_ptr<io_broker>            broker;
  std::unique_ptr<mac_clock_controller> timer_ctrl;
  dummy_f1c_test_client                 cu_notifier;
  gtpu_teid_pool_impl f1u_teid_allocator{MAX_NOF_DU_UES * MAX_NOF_DRBS, GTPU_DEFAULT_TEID_RELEASE_LINGER_TIME, timers};
  cu_up_simulator     cu_up_sim;
  dummy_du_metrics_notifier du_metrics;

  du_high_configuration    du_high_cfg;
  du_high_dependencies     du_hi_dependencies;
  std::unique_ptr<du_high> du_hi;
  phy_test_dummy           phy;
  null_mac_pcap            mac_pcap;
  null_rlc_pcap            rlc_pcap;

  slot_point_extended next_slot;

  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");

protected:
  struct ue_sim_context {
    struct srb_context {
      uint32_t next_pdcp_sn = 0;
    };

    rnti_t                                rnti = rnti_t::INVALID_RNTI;
    std::optional<gnb_du_ue_f1ap_id_t>    du_ue_id;
    std::optional<gnb_cu_ue_f1ap_id_t>    cu_ue_id;
    du_cell_index_t                       pcell_index;
    std::array<srb_context, MAX_NOF_SRBS> srbs;
    std::unique_ptr<f1ap_message>         last_ul_f1ap_msg;
    std::unique_ptr<du_high_ue_simulator> sim;
  };

  /// Handle any pending tasks that should be run before the start of the slot (e.g. RLC PDUs from UEs).
  void handle_slot_preamble_tasks();

  [[nodiscard]] bool
  await_dl_msg_sched(const ue_sim_context& u, lcid_t lcid, std::optional<unsigned> max_slot_count = std::nullopt);

  [[nodiscard]] bool send_dl_rrc_msg_and_await_ul_rrc_msg(const ue_sim_context& u, const f1ap_message& dl_msg);

  /// \brief Launch non-blocking task that on every slot checks the given condition and returns true if it is met
  /// within the given number of slots, false otherwise.
  async_task<bool> launch_run_until_task(unique_function<bool()> condition,
                                         std::optional<unsigned> max_slot_count = std::nullopt);

  /// Launch non-blocking task to await for a PDSCH to be scheduled in the MAC for the given UE and LCID.
  async_task<bool> launch_await_dl_msg_sched_task(const ue_sim_context&   u,
                                                  lcid_t                  lcid,
                                                  std::optional<unsigned> max_slot_count = std::nullopt);

  /// \brief Launch non-blocking task to await for a DL RRC container to be sent to a UE and for the corresponding UL
  /// response to be sent by the DU to the CU-CP.
  async_task<bool> launch_send_dl_rrc_msg_and_await_ul_rrc_msg_task(const ue_sim_context& u,
                                                                    const f1ap_message&   dl_msg);

  std::unordered_map<rnti_t, ue_sim_context> ues;

  unsigned next_cu_ue_id = 0;

  // Asynchronous tasks of the simulator.
  std::list<eager_async_task<void>> pending_tasks;
  event_signal_flag                 next_slot_signal;
};

} // namespace odu
} // namespace ocudu
